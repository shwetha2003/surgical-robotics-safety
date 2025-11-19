#include "command_subscriber.h"
#include <iostream>

CommandSubscriber::CommandSubscriber() : participant(0) {
    try {
        // Create topic for receiving commands
        dds::core::QosProvider qos_provider("config/dds_config.xml");
        
        dds::topic::qos::TopicQos topic_qos = qos_provider.topic_qos("MedicalDeviceQoS");
        topic = dds::topic::Topic<ControlCommand>(participant, "SurgicalControlCommands", topic_qos);
        
        dds::sub::qos::DataReaderQos reader_qos = qos_provider.datareader_qos("RealTimeDataQoS");
        reader = dds::sub::DataReader<ControlCommand>(dds::sub::Subscriber(participant), topic, reader_qos);
        
        // Start command processing thread
        command_thread = std::thread(&CommandSubscriber::processCommands, this);
        
        std::cout << "DDS Command Subscriber Initialized Successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "DDS Subscriber Initialization Error: " << e.what() << std::endl;
        throw;
    }
}

void CommandSubscriber::processCommands() {
    while (!shutdown_requested) {
        try {
            dds::sub::LoanedSamples<ControlCommand> samples = reader.take();
            
            for (const auto& sample : samples) {
                if (sample.info().valid()) {
                    handleControlCommand(sample.data());
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
        } catch (const std::exception& e) {
            std::cerr << "Command processing error: " << e.what() << std::endl;
        }
    }
}

void CommandSubscriber::handleControlCommand(const ControlCommand& command) {
    std::string command_type = command.command_type();
    
    if (command_type == "EMERGENCY_STOP") {
        std::cout << "Received EMERGENCY STOP command: " << command.reason() << std::endl;
        // Trigger emergency stop in safety monitor
        safety_monitor->triggerEmergencyStop(command.reason());
        
    } else if (command_type == "RESUME_OPERATION") {
        std::cout << "Received RESUME OPERATION command" << std::endl;
        safety_monitor->resumeNormalOperation();
        
    } else if (command_type == "FORCE_LIMIT_ADJUST") {
        std::cout << "Received force limit adjustment: " << command.value() << "N" << std::endl;
        safety_monitor->adjustForceLimit(command.value());
        
    } else {
        std::cout << "Received unknown command: " << command_type << std::endl;
    }
}

void CommandSubscriber::shutdown() {
    shutdown_requested = true;
    if (command_thread.joinable()) {
        command_thread.join();
    }
}
