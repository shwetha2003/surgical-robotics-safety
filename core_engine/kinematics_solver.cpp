#include "kinematics_solver.h"
#include <iostream>
#include <cmath>
#include <stdexcept>

RoboticsKinematics::RoboticsKinematics() {
    // Initialize with Ethicon surgical robot DH parameters
    // [theta, alpha, a, d] for each joint
    dh_parameters = {
        0.0, M_PI/2, 0.0, 0.15,      // Joint 1
        0.0, -M_PI/2, 0.25, 0.0,     // Joint 2  
        0.0, M_PI/2, 0.0, 0.18,      // Joint 3
        0.0, -M_PI/2, 0.0, 0.0,      // Joint 4
        0.0, M_PI/2, 0.0, 0.1,       // Joint 5
        0.0, 0.0, 0.0, 0.05          // Joint 6 (end effector)
    };
    
    base_transform = Eigen::Matrix4d::Identity();
}

Eigen::Vector3d RoboticsKinematics::forwardKinematics(const std::vector<double>& joint_angles) {
    if(joint_angles.size() != 6) {
        throw std::invalid_argument("Expected 6 joint angles");
    }
    
    Eigen::Matrix4d T = base_transform;
    
    for(size_t i = 0; i < joint_angles.size(); ++i) {
        double theta = joint_angles[i] + dh_parameters[i*4];
        double alpha = dh_parameters[i*4+1];
        double a = dh_parameters[i*4+2];
        double d = dh_parameters[i*4+3];
        
        Eigen::Matrix4d Ti = calculateTransform(theta, alpha, a, d);
        T = T * Ti;
    }
    
    return T.block<3,1>(0,3); // Return position vector
}

std::vector<double> RoboticsKinematics::inverseKinematics(const Eigen::Vector3d& target_position) {
    // Analytical IK solution for 6-DOF surgical robot
    std::vector<double> joint_angles(6, 0.0);
    
    double x = target_position[0];
    double y = target_position[1]; 
    double z = target_position[2];
    
    // Solve for first three joints (positioning)
    joint_angles[0] = atan2(y, x);
    
    double r = sqrt(x*x + y*y);
    double D = (r*r + z*z - dh_parameters[2]*dh_parameters[2] - dh_parameters[6]*dh_parameters[6]) 
               / (2 * dh_parameters[2] * dh_parameters[6]);
    
    if(D < -1.0 || D > 1.0) {
        throw std::runtime_error("Target position unreachable");
    }
    
    joint_angles[2] = atan2(sqrt(1 - D*D), D);
    joint_angles[1] = atan2(z, r) - atan2(dh_parameters[6] * sin(joint_angles[2]), 
                                     dh_parameters[2] + dh_parameters[6] * cos(joint_angles[2]));
    
    // Last three joints for orientation (simplified)
    joint_angles[3] = 0.0; // Roll
    joint_angles[4] = M_PI/2; // Pitch  
    joint_angles[5] = 0.0; // Yaw
    
    return validateJointSolution(joint_angles);
}

Eigen::MatrixXd RoboticsKinematics::calculateJacobian(const std::vector<double>& joint_angles) {
    // Remove unused parameter warning by using the parameter
    (void)joint_angles; // Mark as used
    
    Eigen::MatrixXd jacobian(6, 6);
    
    // Calculate Jacobian matrix for velocity control
    // This is a simplified version - real implementation would be more complex
    for(int i = 0; i < 6; ++i) {
        for(int j = 0; j < 6; ++j) {
            if(i == j) {
                jacobian(i, j) = 1.0; // Simplified diagonal
            } else {
                jacobian(i, j) = 0.0;
            }
        }
    }
    
    return jacobian;
}

bool RoboticsKinematics::validateSolution(const std::vector<double>& joint_angles) {
    for(double angle : joint_angles) {
        if(std::isnan(angle) || std::isinf(angle)) {
            return false;
        }
    }
    return true;
}

bool RoboticsKinematics::isReachable(const Eigen::Vector3d& target_position) {
    try {
        auto solution = inverseKinematics(target_position);
        return validateSolution(solution);
    } catch(const std::exception& e) {
        return false;
    }
}

std::vector<double> RoboticsKinematics::validateJointSolution(const std::vector<double>& joint_angles) {
    std::vector<double> validated_angles = joint_angles;
    
    // Normalize angles to [-pi, pi]
    for(double& angle : validated_angles) {
        while(angle > M_PI) angle -= 2 * M_PI;
        while(angle < -M_PI) angle += 2 * M_PI;
    }
    
    return validated_angles;
}

Eigen::Matrix4d RoboticsKinematics::calculateTransform(double theta, double alpha, double a, double d) {
    Eigen::Matrix4d T;
    T << cos(theta), -sin(theta)*cos(alpha),  sin(theta)*sin(alpha), a*cos(theta),
         sin(theta),  cos(theta)*cos(alpha), -cos(theta)*sin(alpha), a*sin(theta),
         0,           sin(alpha),             cos(alpha),            d,
         0,           0,                      0,                     1;
    return T;
}
