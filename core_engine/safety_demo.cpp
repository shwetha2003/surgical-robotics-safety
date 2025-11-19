#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "safety_monitor.h"

int main() {
    std::cout << "🚀 J&J Surgical Robotics Safety Platform - Demo" << std::endl;
    std::cout << "==============================================" << std::endl;
    
    // Initialize safety monitor
    SurgicalSafetyMonitor safety_monitor;
    
    // Simulate surgical procedure
    std::vector<std::vector<double>> test_positions = {
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // Valid position
        {100.0, 50.0, 30.0, 20.0, 10.0, 5.0}, // Valid position  
        {200.0, 0.0, 0.0, 0.0, 0.0, 0.0}     // Invalid position (exceeds limits)
    };
    
    std::vector<std::vector<double>> test_forces = {
        {5.0, 5.0, 5.0},    // Valid forces
        {12.0, 8.0, 6.0},   // Valid but high
        {18.0, 5.0, 5.0}    // Invalid (exceeds 15N limit)
    };
    
    // Test safety validation
    for (size_t i = 0; i < test_positions.size(); ++i) {
        std::cout << "\n📊 Test " << (i + 1) << ":" << std::endl;
        
        bool position_ok = safety_monitor.validateJointPosition(test_positions[i]);
        bool force_ok = safety_monitor.validateForceReadings(test_forces[i]);
        
        std::cout << "Position validation: " << (position_ok ? "✅ PASS" : "❌ FAIL") << std::endl;
        std::cout << "Force validation: " << (force_ok ? "✅ PASS" : "❌ FAIL") << std::endl;
        std::cout << "Safety Score: " << safety_monitor.calculateOverallSafetyScore() << "%" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    // Demonstrate emergency stop
    std::cout << "\n🛑 Testing Emergency Stop..." << std::endl;
    safety_monitor.triggerEmergencyStop("DEMO_EMERGENCY");
    std::cout << "Emergency Stop Engaged: " << (safety_monitor.isEmergencyStopEngaged() ? "YES" : "NO") << std::endl;
    
    std::cout << "\n✅ Demo completed successfully!" << std::endl;
    return 0;
}
