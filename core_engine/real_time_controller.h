#ifndef REAL_TIME_CONTROLLER_H
#define REAL_TIME_CONTROLLER_H

#include <thread>
#include <atomic>

class RealTimeController {
private:
    std::thread control_thread;
    std::atomic<bool> is_running;
    int control_frequency;
    unsigned long cycle_count;
    
    void controlLoop();
    void executeControlCycle();
    void readSensorData();
    void performSafetyChecks();
    void sendControlCommands();
    
public:
    RealTimeController();
    ~RealTimeController();
    
    void startControlLoop();
    void stopControlLoop();
    void setControlFrequency(int frequency);
    
    bool isRunning() const { return is_running; }
    int getControlFrequency() const { return control_frequency; }
    unsigned long getCycleCount() const { return cycle_count; }
};

#endif // REAL_TIME_CONTROLLER_H
