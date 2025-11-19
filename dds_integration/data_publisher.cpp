#include "data_publisher.h"
#include <iostream>
#include <chrono>

RoboticsDataPublisher::RoboticsDataPublisher() : participant(0) {
    try {
        // Create topic with medical device QoS
        dds::core::QosProvider qos_provider("config/dds_config.xml");
        
        dds::topic::qos::TopicQos topic_qos = qos_provider.topic_qos("MedicalDeviceQoS");
        topic = dds::topic::Topic<RoboticsData>(participant, "SurgicalRoboticsData", topic_qos);
        
        dds::pub::qos::DataWriterQos writer_qos = qos_provider.datawriter_qos("RealTimeDataQoS");
        writer = dds::pub::DataWriter<RoboticsData>(dds::pub::Publisher(participant), topic, writer_qos);
        
        std::cout << "DDS Data Publisher Initialized Successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "DDS Initialization Error: " << e.what() << std::endl;
        throw;
    }
}

void RoboticsDataPublisher::publishSafetyData(const SafetyMetrics& metrics) {
    try {
        RoboticsData sample;
        
        // Set timestamp
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        sample.timestamp(timestamp);
        
        // Set joint data
        sample.joint_positions(metrics.joint_positions);
        sample.joint_velocities(metrics.joint_velocities);
        
        // Set force data
        sample.force_readings(metrics.force_readings);
        
        // Set safety status
        sample.safety_status(metrics.safety_status);
        sample.collision_risk(metrics.collision_risk);
        sample.emergency_stop_engaged(metrics.emergency_stop);
        
        // Set procedure information
        sample.procedure_phase(metrics.procedure_phase);
        sample.procedure_duration(metrics.procedure_duration);
        
        // Publish with medical-grade reliability
        writer.write(sample);
        
        // Log successful publication
        if(timestamp % 5000 == 0) { // Log every 5 seconds
            std::cout << "Safety data published - Safety Score: " << metrics.safety_score 
                      << ", Emergency Stop: " << (metrics.emergency_stop ? "YES" : "NO") << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "DDS Publication Error: " << e.what() << std::endl;
        // In medical context, might trigger safety alert
    }
}

void RoboticsDataPublisher::publishEmergencyStop(const std::string& reason) {
    try {
        EmergencyCommand command;
        command.timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        command.command_type("EMERGENCY_STOP");
        command.reason(reason);
        command.severity("CRITICAL");
        command.requires_acknowledgment(true);
        
        writer.write(command);
        std::cout << "EMERGENCY STOP COMMAND PUBLISHED: " << reason << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "CRITICAL: Failed to publish emergency stop: " << e.what() << std::endl;
    }
}

void RoboticsDataPublisher::publishSafetyAlert(const SafetyAlert& alert) {
    try {
        SafetyAlertData alert_sample;
        alert_sample.alert_id(alert.alert_id);
        alert_sample.timestamp(alert.timestamp);
        alert_sample.severity(alert.severity);
        alert_sample.message(alert.message);
        alert_sample.component(alert.component);
        alert_sample.recommended_action(alert.recommended_action);
        
        writer.write(alert_sample);
        
    } catch (const std::exception& e) {
        std::cerr << "Safety alert publication failed: " << e.wwhat() << std::endl;
    }
}
