#include <gtest/gtest.h>
#include "../core_engine/safety_monitor.h"

class SafetyMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        monitor = std::make_unique<SurgicalSafetyMonitor>();
    }
    
    std::unique_ptr<SurgicalSafetyMonitor> monitor;
};

TEST_F(SafetyMonitorTest, ValidJointPositions) {
    std::vector<double> valid_positions = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    EXPECT_TRUE(monitor->validateJointPosition(valid_positions));
}

TEST_F(SafetyMonitorTest, InvalidJointPositions) {
    std::vector<double> invalid_positions = {200.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // Exceeds 180 limit
    EXPECT_FALSE(monitor->validateJointPosition(invalid_positions));
}

TEST_F(SafetyMonitorTest, ValidForceReadings) {
    std::vector<double> valid_forces = {5.0, 5.0, 5.0};
    EXPECT_TRUE(monitor->validateForceReadings(valid_forces));
}

TEST_F(SafetyMonitorTest, ExcessiveForceTriggersSafety) {
    std::vector<double> excessive_forces = {20.0, 5.0, 5.0}; // 20N exceeds 15N limit
    EXPECT_FALSE(monitor->validateForceReadings(excessive_forces));
}

TEST_F(SafetyMonitorTest, EmergencyStopEngagement) {
    monitor->triggerEmergencyStop("TEST_EMERGENCY");
    EXPECT_TRUE(monitor->isEmergencyStopEngaged());
}

TEST_F(SafetyMonitorTest, SafetyScoreCalculation) {
    // Test that safety score decreases with violations
    double initial_score = monitor->calculateOverallSafetyScore();
    
    // Trigger some safety events
    std::vector<double> excessive_forces = {20.0, 5.0, 5.0};
    monitor->validateForceReadings(excessive_forces);
    
    double new_score = monitor->calculateOverallSafetyScore();
    EXPECT_LT(new_score, initial_score);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
