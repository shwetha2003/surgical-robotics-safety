#include "real_time_controller.h"
#include <iostream>
#include <chrono>
#include <thread>

RealTimeController::RealTimeController() : is_running(false), control_frequency(1000) {
    std::cout << "RealTimeController initialized with " << control_frequency << "Hz frequency" << std::endl;
}

void RealTimeController::startControlLoop() {
    if (is_running) {
        std::cout << "Control loop already running" << std::endl;
        return;
    }
    
    is_running = true;
    control_thread = std::thread(&RealTimeController::controlLoop, this);
    std::cout << "Control loop started" << std::endl;
}

void RealTimeController::stopControlLoop() {
    is_running = false;
    if (control_thread.joinable()) {
        control_thread.join();
    }
    std::cout << "Control loop stopped" << std::endl;
}

void RealTimeController::controlLoop() {
    auto control_interval = std::chrono::microseconds(1000000 / control_frequency);
    
    while (is_running) {
        auto loop_start = std::chrono::high_resolution_clock::now();
        
        // Execute one control cycle
        executeControlCycle();
        
        auto loop_end = std::chrono::high_resolution_clock::now();
        auto loop_duration = std::chrono::duration_cast<std::chrono::microseconds>(loop_end - loop_start);
        
        // Maintain real-time performance
        if (loop_duration < control_interval) {
            std::this_thread::sleep_for(control_interval - loop_duration);
        } else {
            std::cout << "⚠️  Control loop timing violation: " << loop_duration.count() << "μs" << std::endl;
        }
        
        cycle_count++;
    }
}

void RealTimeController::executeControlCycle() {
    // This is where the actual control logic would run
    // For demonstration, we'll just simulate some work
    
    // Simulate reading sensor data
    readSensorData();
    
    // Simulate safety checks
    performSafetyChecks();
    
    // Simulate sending control commands
    sendControlCommands();
    
    // Log performance occasionally
    if (cycle_count % 1000 == 0) {
        std::cout << "Control cycle " << cycle_count << " completed" << std::endl;
    }
}

void RealTimeController::readSensorData() {
    // Simulate reading from force sensors, encoders, etc.
    // In real implementation, this would interface with hardware
}

void RealTimeController::performSafetyChecks() {
    // Simulate safety monitoring
    // In real implementation, this would use the SafetyMonitor
}

void RealTimeController::sendControlCommands() {
    // Simulate sending commands to motors/actuators
    // In real implementation, this would interface with hardware
}

void RealTimeController::setControlFrequency(int frequency) {
    if (frequency > 0 && frequency <= 10000) {  // Reasonable limits
        control_frequency = frequency;
        std::cout << "Control frequency set to " << control_frequency << "Hz" << std::endl;
    } else {
        std::cout << "Invalid control frequency: " << frequency << "Hz" << std::endl;
    }
}

RealTimeController::~RealTimeController() {
    stopControlLoop();
}
