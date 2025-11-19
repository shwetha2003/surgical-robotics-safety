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
    while (!shutdown_requested) {
        // Simulate receiving commands
        // In real implementation, this would use DDS to receive data
        
        // Simulate occasional command reception
        static int counter = 0;
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
              << " - " << command.reason << std::endl;
}

void CommandSubscriber::shutdown() {
    shutdown_requested = true;
    if (command_thread.joinable()) {
        command_thread.join();
    }
    std::cout << "DDS Command Subscriber Shutdown" << std::endl;
}
