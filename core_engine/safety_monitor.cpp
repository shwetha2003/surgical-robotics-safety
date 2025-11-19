#include "safety_monitor.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <limits>

SurgicalSafetyMonitor::SurgicalSafetyMonitor() : emergency_stop_engaged(false) {
    initializeSafetyParameters();
}

void SurgicalSafetyMonitor::initializeSafetyParameters() {
    // Initialize with Ethicon surgical robot specifications
    joint_limits = {-180.0, 180.0, -90.0, 90.0, -120.0, 120.0, -150.0, 150.0};
    force_limits = {MAX_FORCE_NEWTONS, MAX_FORCE_NEWTONS, MAX_FORCE_NEWTONS};
    velocity_limits = {MAX_VELOCITY_MM_PER_SEC, MAX_VELOCITY_MM_PER_SEC, MAX_VELOCITY_MM_PER_SEC};
    
    std::cout << "Safety Monitor Initialized with IEC 62304 Compliance" << std::endl;
}

bool SurgicalSafetyMonitor::validateJointPosition(const std::vector<double>& positions) {
    std::lock_guard<std::mutex> lock(safety_mutex);
    
    if(positions.size() * 2 != joint_limits.size()) {
        logSafetyEvent("INVALID_JOINT_DATA", static_cast<double>(positions.size()));
        return false;
    }
    
    for(size_t i = 0; i < positions.size(); ++i) {
        if(positions[i] < joint_limits[i*2] || positions[i] > joint_limits[i*2+1]) {
            triggerEmergencyStop("JOINT_LIMIT_EXCEEDED");
            logSafetyEvent("JOINT_SAFETY_VIOLATION", positions[i]);
            return false;
        }
    }
    return true;
}

bool SurgicalSafetyMonitor::validateForceReadings(const std::vector<double>& forces) {
    std::lock_guard<std::mutex> lock(safety_mutex);
    
    for(size_t i = 0; i < forces.size(); ++i) {
        if(forces[i] > force_limits[i]) {
            triggerForceReduction(forces[i], force_limits[i]);
            logSafetyEvent("EXCESSIVE_FORCE", forces[i]);
            return false;
        }
        
        // Check for rapid force changes (potential tissue damage)
        if(i > 0 && std::abs(forces[i] - forces[i-1]) > 5.0) {
            logSafetyEvent("RAPID_FORCE_CHANGE", std::abs(forces[i] - forces[i-1]));
        }
    }
    return true;
}

bool SurgicalSafetyMonitor::validateVelocity(const std::vector<double>& velocities) {
    for(size_t i = 0; i < velocities.size(); ++i) {
        if(std::abs(velocities[i]) > velocity_limits[i]) {
            logSafetyEvent("EXCESSIVE_VELOCITY", velocities[i]);
            return false;
        }
    }
    return true;
}

bool SurgicalSafetyMonitor::checkCollisionRisk(const std::vector<double>& positions,
                                             const std::vector<std::vector<double>>& obstacles) {
    // Simplified collision detection - in practice would use 3D geometry
    for(const auto& obstacle : obstacles) {
        if(obstacle.size() != positions.size()) continue;
        
        double distance_sq = 0.0;
        for(size_t i = 0; i < positions.size(); ++i) {
            distance_sq += std::pow(positions[i] - obstacle[i], 2);
        }
        
        double distance = std::sqrt(distance_sq);
        if(distance < MIN_SAFE_DISTANCE_MM) {
            logSafetyEvent("COLLISION_IMMINENT", distance);
            return true;
        }
    }
    return false;
}

void SurgicalSafetyMonitor::triggerEmergencyStop(const std::string& reason) {
    std::lock_guard<std::mutex> lock(safety_mutex);
    emergency_stop_engaged = true;
    
    logSafetyEvent("EMERGENCY_STOP_TRIGGERED", 0.0);
    std::cout << "EMERGENCY STOP: " << reason << std::endl;
    
    sendStopCommandToHardware();
}

void SurgicalSafetyMonitor::triggerForceReduction(double current_force, double max_force) {
    logSafetyEvent("FORCE_REDUCTION_APPLIED", current_force);
    std::cout << "Force reduction: " << current_force << "N exceeds " << max_force << "N limit" << std::endl;
}

void SurgicalSafetyMonitor::resumeNormalOperation() {
    std::lock_guard<std::mutex> lock(safety_mutex);
    emergency_stop_engaged = false;
    logSafetyEvent("NORMAL_OPERATION_RESUMED", 0.0);
}

void SurgicalSafetyMonitor::logSafetyEvent(const std::string& event_type, double value) {
    SafetyEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.event_type = event_type;
    event.value = value;
    event.robot_state = getCurrentRobotState();
    event.severity_level = (event_type.find("EMERGENCY") != std::string::npos) ? 5 : 3;
    
    safety_event_queue.push(event);
    
    // Maintain queue size - fix signed/unsigned comparison
    if(safety_event_queue.size() > static_cast<size_t>(MAX_SAFETY_EVENTS)) {
        safety_event_queue.pop();
    }
}

std::vector<SafetyEvent> SurgicalSafetyMonitor::getRecentSafetyEvents(int count) {
    std::lock_guard<std::mutex> lock(safety_mutex);
    std::vector<SafetyEvent> recent_events;
    
    // Create temporary queue to access elements
    std::queue<SafetyEvent> temp_queue = safety_event_queue;
    int events_to_get = std::min(count, static_cast<int>(temp_queue.size()));
    
    for(int i = 0; i < events_to_get; ++i) {
        recent_events.push_back(temp_queue.front());
        temp_queue.pop();
    }
    
    return recent_events;
}

double SurgicalSafetyMonitor::calculateOverallSafetyScore() const {
    // Calculate safety score based on recent events
    // Create a temporary non-const copy for getRecentSafetyEvents call
    SurgicalSafetyMonitor* non_const_this = const_cast<SurgicalSafetyMonitor*>(this);
    auto recent_events = non_const_this->getRecentSafetyEvents(100);
    
    if(recent_events.empty()) return 100.0;
    
    double penalty_score = 0.0;
    for(const auto& event : recent_events) {
        penalty_score += event.severity_level * 0.5;
    }
    
    return std::max(0.0, 100.0 - penalty_score);
}

void SurgicalSafetyMonitor::sendStopCommandToHardware() {
    // In real implementation, this would interface with robot hardware
    std::cout << "Sending STOP command to surgical robot hardware..." << std::endl;
}

std::string SurgicalSafetyMonitor::getCurrentRobotState() const {
    return emergency_stop_engaged ? "EMERGENCY_STOP" : "OPERATIONAL";
}
