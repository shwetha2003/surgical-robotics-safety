#ifndef COMMAND_SUBSCRIBER_H
#define COMMAND_SUBSCRIBER_H

#include <thread>
#include <atomic>
#include <string>  // Add missing include

// Simplified control command structure
struct ControlCommand {
    long timestamp;
    std::string command_type;
    std::string reason;
    double value;
};

class CommandSubscriber {
private:
    std::thread command_thread;
    std::atomic<bool> shutdown_requested;
    
    void processCommands();
    void handleControlCommand(const ControlCommand& command);
    
public:
    CommandSubscriber();
    ~CommandSubscriber();
    void shutdown();
};

#endif // COMMAND_SUBSCRIBER_H
