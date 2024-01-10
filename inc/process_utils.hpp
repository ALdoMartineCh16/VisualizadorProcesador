#ifndef __PROCESS_UTILS_H__
#define __PROCESS_UTILS_H__

#include "process.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <unistd.h>
#include <filesystem>
#include <algorithm>
#include <iomanip>

namespace fs = std::filesystem;

void getProcesses(std::vector<ProcessInfo> &processes);
void showProcesses(const std::vector<ProcessInfo>& processes);
void sortByArrivalTime(std::vector<ProcessInfo>& processes);

#endif // __PROCESS_UTILS_H__