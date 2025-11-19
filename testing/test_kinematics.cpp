#include <gtest/gtest.h>
#include "../core_engine/kinematics_solver.h"

class KinematicsTest : public ::testing::Test {
protected:
    void SetUp() override {
        solver = std::make_unique<RoboticsKinematics>();
    }
    
    std::unique_ptr<RoboticsKinematics> solver;
};

TEST_F(KinematicsTest, ForwardKinematicsValidInput) {
    std::vector<double> joint_angles = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    auto position = solver->forwardKinematics(joint_angles);
    
    // Should return valid position
    EXPECT_FALSE(position.hasNaN());
    EXPECT_FALSE(position.array().isInf().any());
}

TEST_F(KinematicsTest, InverseKinematicsReachableTarget) {
    Eigen::Vector3d target(0.3, 0.2, 0.1); // Reachable position
    auto joint_angles = solver->inverseKinematics(target);
    
    EXPECT_EQ(joint_angles.size(), 6);
    EXPECT_TRUE(solver->validateSolution(joint_angles));
}

TEST_F(KinematicsTest, ReachabilityCheck) {
    Eigen::Vector3d reachable(0.3, 0.2, 0.1);
    Eigen::Vector3d unreachable(10.0, 10.0, 10.0); // Far outside workspace
    
    EXPECT_TRUE(solver->isReachable(reachable));
    EXPECT_FALSE(solver->isReachable(unreachable));
}
