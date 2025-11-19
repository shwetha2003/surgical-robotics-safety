#include "command_subscriber.h"
#include <iostream>
#include <chrono>
#include <thread>

CommandSubscriber::CommandSubscriber() : shutdown_requested(false) {
    std::cout << "DDS Command Subscriber Initialized (Simulation Mode)" << std::endl;
    
    // Start command processing thread
    command_thread = std::thread(&CommandSubscriber::processCommands, this);
}

CommandSubscriber::~CommandSubscriber() {
    shutdown();
}

void CommandSubscriber::processCommands() {
    int counter = 0;
    while (!shutdown_requested) {
        // Simulate receiving commands periodically
        if (counter++ % 100 == 0) {
            ControlCommand simulated_command;
            simulated_command.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            simulated_command.command_type = "STATUS_CHECK";
            simulated_command.reason = "Periodic system check";
            simulated_command.value = 1.0;
            
            handleControlCommand(simulated_command);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void CommandSubscriber::handleControlCommand(const ControlCommand& command) {
    std::cout << "📥 DDS Command Received: " << command.command_type 
              << " - " << command.reason << " (Value: " << command.value << ")" << std::endl;
    
    // Handle different command types
    if (command.command_type == "EMERGENCY_STOP") {
        std::cout << "🚨 Processing EMERGENCY STOP command" << std::endl;
    } else if (command.command_type == "RESUME_OPERATION") {
        std::cout << "🔄 Processing RESUME OPERATION command" << std::endl;
    } else if (command.command_type == "FORCE_LIMIT_ADJUST") {
        std::cout << "⚙️  Processing force limit adjustment: " << command.value << "N" << std::endl;
    } else if (command.command_type == "STATUS_CHECK") {
        std::cout << "📊 Processing status check command" << std::endl;
    } else {
        std::cout << "❓ Unknown command type: " << command.command_type << std::endl;
    }
}

void CommandSubscriber::shutdown() {
    shutdown_requested = true;
    if (command_thread.joinable()) {
        command_thread.join();
    }
    std::cout << "DDS Command Subscriber Shutdown" << std::endl;
}
