#include "data_publisher.h"
#include <iostream>
#include <chrono>

RoboticsDataPublisher::RoboticsDataPublisher() {
    std::cout << "DDS Data Publisher Initialized (Simulation Mode)" << std::endl;
}

void RoboticsDataPublisher::publishSafetyData(const SafetyMetrics& metrics) {
    // Simulate DDS publication
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    std::cout << "📡 DDS Publishing Safety Data - Timestamp: " << timestamp 
              << ", Safety Score: " << metrics.safety_score << std::endl;
}

void RoboticsDataPublisher::publishEmergencyStop(const std::string& reason) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    std::cout << "🚨 DDS EMERGENCY STOP - Reason: " << reason 
              << ", Timestamp: " << timestamp << std::endl;
}

void RoboticsDataPublisher::publishSafetyAlert(const SafetyAlert& alert) {
    std::cout << "⚠️  DDS Safety Alert - " << alert.severity << ": " << alert.message 
              << " [Component: " << alert.component << "]" << std::endl;
}
