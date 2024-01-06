#ifndef PROCESS_H
#define PROCESS_H

#include <string.h>
using namespace std;
class ProcessInfo {
public:
    char name[256];
    int pid;
    int priority;
    int burst_time;
    int arrival_time;
    int left_time;
    int waiting_time;
    int response_time;

    ProcessInfo(){}
    ProcessInfo(char *name, int pid, int priority, int burst_time, int arrival_time) {
        strcpy(this->name, name);
        this->pid = pid;
        this->priority = priority;
        this->burst_time = burst_time;
        this->arrival_time = arrival_time;
        this->left_time = burst_time;
        this->waiting_time = 0;
        this->response_time = 0;
    }
    void InitProcessInfo(char *name, int pid, int priority, int burst_time, int arrival_time) {
        strcpy(this->name, name);
        this->pid = pid;
        this->priority = priority;
        this->burst_time = burst_time;
        this->arrival_time = arrival_time;
        this->left_time = burst_time;
        this->waiting_time = 0;
        this->response_time = 0;
    }
};


#endif
