#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <string>

class ProcessInfo {
public:
    std::string name;
    int pid;
    int priority;           // -20 (high) to 19 (low)
    double burst_time;      // nanosec
    double arrival_time;    // sec
    double left_time;       // nanosec
    double waiting_time;    // nanosec
    double response_time;   // nanosec

    ProcessInfo() = default;

    ProcessInfo(const std::string& name, int pid, int priority, double burst_time, double arrival_time) : 
        name(name), 
        pid(pid), 
        priority(priority), 
        burst_time(burst_time), 
        arrival_time(arrival_time), 
        left_time(burst_time), 
        waiting_time(0), 
        response_time(0) {}
};

#endif // __PROCESS_H__
