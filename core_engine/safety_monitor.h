#ifndef SAFETY_MONITOR_H
#define SAFETY_MONITOR_H

#include <vector>
#include <string>
#include <chrono>
#include <mutex>
#include <queue>

struct SafetyEvent {
    std::chrono::system_clock::time_point timestamp;
    std::string event_type;
    double value;
    std::string robot_state;
    int severity_level; // 1-5, 5 being most critical
};

class SurgicalSafetyMonitor {
private:
    std::mutex safety_mutex;
    bool emergency_stop_engaged;
    std::vector<double> joint_limits;
    std::vector<double> force_limits;
    std::vector<double> velocity_limits;
    std::queue<SafetyEvent> safety_event_queue;
    
    // IEC 62304 Critical Safety Parameters
    const double MAX_FORCE_NEWTONS = 15.0;
    const double MAX_VELOCITY_MM_PER_SEC = 50.0;
    const double MIN_SAFE_DISTANCE_MM = 2.0;
    const int MAX_SAFETY_EVENTS = 10000;
    
public:
    SurgicalSafetyMonitor();
    ~SurgicalSafetyMonitor() = default;
    
    // Core safety validation methods
    bool validateJointPosition(const std::vector<double>& positions);
    bool validateForceReadings(const std::vector<double>& forces);
    bool validateVelocity(const std::vector<double>& velocities);
    bool checkCollisionRisk(const std::vector<double>& positions, 
                          const std::vector<std::vector<double>>& obstacles);
    
    // Emergency procedures
    void triggerEmergencyStop(const std::string& reason);
    void triggerForceReduction(double current_force, double max_force);
    void resumeNormalOperation();
    
    // Monitoring and logging
    void logSafetyEvent(const std::string& event_type, double value = 0.0);
    std::vector<SafetyEvent> getRecentSafetyEvents(int count = 10);
    double calculateOverallSafetyScore() const;
    
    // Getters
    bool isEmergencyStopEngaged() const { return emergency_stop_engaged; }
    std::vector<double> getCurrentLimits() const { return force_limits; }
    
private:
    void sendStopCommandToHardware();
    void initializeSafetyParameters();
    std::string getCurrentRobotState() const;
};

#endif // SAFETY_MONITOR_H
