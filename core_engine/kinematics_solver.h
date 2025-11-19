#ifndef KINEMATICS_SOLVER_H
#define KINEMATICS_SOLVER_H

#include <vector>
#include <eigen3/Eigen/Dense>

class RoboticsKinematics {
private:
    std::vector<double> dh_parameters; // Denavit-Hartenberg parameters
    Eigen::Matrix4d base_transform;
    
public:
    RoboticsKinematics();
    
    // Forward kinematics: joint angles -> end effector position
    Eigen::Vector3d forwardKinematics(const std::vector<double>& joint_angles);
    
    // Inverse kinematics: target position -> joint angles
    std::vector<double> inverseKinematics(const Eigen::Vector3d& target_position);
    
    // Jacobian calculation for velocity control
    Eigen::MatrixXd calculateJacobian(const std::vector<double>& joint_angles);
    
    // Validation methods
    bool validateSolution(const std::vector<double>& joint_angles);
    bool isReachable(const Eigen::Vector3d& target_position);
    
private:
    std::vector<double> validateJointSolution(const std::vector<double>& joint_angles);
    Eigen::Matrix4d calculateTransform(double theta, double alpha, double a, double d);
};

#endif // KINEMATICS_SOLVER_H
