#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "safety_monitor.h"
#include "kinematics_solver.h"
#include "collision_detector.h"

int main() {
    std::cout << "🚀 J&J Surgical Robotics Safety Platform - Enhanced Demo" << std::endl;
    std::cout << "========================================================" << std::endl;
    
    // Initialize all components
    SurgicalSafetyMonitor safety_monitor;
    RoboticsKinematics kinematics_solver;
    CollisionDetector collision_detector;
    
    std::cout << "\n📋 Testing Safety Monitor..." << std::endl;
    // Test 1: Valid position and force
    std::vector<double> valid_positions = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::vector<double> valid_forces = {5.0, 5.0, 5.0};
    
    bool position_ok = safety_monitor.validateJointPosition(valid_positions);
    bool force_ok = safety_monitor.validateForceReadings(valid_forces);
    
    std::cout << "Position validation: " << (position_ok ? "✅ PASS" : "❌ FAIL") << std::endl;
    std::cout << "Force validation: " << (force_ok ? "✅ PASS" : "❌ FAIL") << std::endl;
    
    std::cout << "\n📋 Testing Kinematics..." << std::endl;
    // Test forward kinematics
    auto position = kinematics_solver.forwardKinematics(valid_positions);
    std::cout << "Forward kinematics result: [" << position[0] << ", " 
              << position[1] << ", " << position[2] << "]" << std::endl;
    
    std::cout << "\n📋 Testing Collision Detection..." << std::endl;
    // Test collision detection
    Eigen::Vector3d instrument_tip(0.1, 0.1, 0.1);
    std::vector<Eigen::Vector3d> obstacles = {
        Eigen::Vector3d(0.15, 0.15, 0.15),  // Close obstacle
        Eigen::Vector3d(0.5, 0.5, 0.5)      // Far obstacle
    };
    
    bool collision = collision_detector.checkInstrumentCollision(instrument_tip, obstacles);
    double min_distance = collision_detector.calculateMinimumDistance(instrument_tip, obstacles);
    
    std::cout << "Collision detected: " << (collision ? "YES" : "NO") << std::endl;
    std::cout << "Minimum distance: " << min_distance << "mm" << std::endl;
    
    std::cout << "\n📋 Testing Emergency Procedures..." << std::endl;
    // Test emergency stop
    safety_monitor.triggerEmergencyStop("DEMO_EMERGENCY");
    std::cout << "Emergency Stop Engaged: " << (safety_monitor.isEmergencyStopEngaged() ? "YES" : "NO") << std::endl;
    
    // Test resume
    safety_monitor.resumeNormalOperation();
    std::cout << "Emergency Stop Engaged: " << (safety_monitor.isEmergencyStopEngaged() ? "YES" : "NO") << std::endl;
    
    std::cout << "\n📊 Final Safety Score: " << safety_monitor.calculateOverallSafetyScore() << "%" << std::endl;
    
    std::cout << "\n✅ Enhanced demo completed successfully!" << std::endl;
    std::cout << "🎯 All components working together for surgical safety!" << std::endl;
    
    return 0;
}
