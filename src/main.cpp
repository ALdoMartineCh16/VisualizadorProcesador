#include <nlohmann/json.hpp>
#include <httplib.h>
#include "process_utils.hpp"

using json = nlohmann::json;

int main()
{
    httplib::Server svr;
    std::cout << json::meta() << std::endl;
    std::vector<ProcessInfo> processes;
    getProcesses(processes);
    std::cout << "Nº processes: " << processes.size() << std::endl;
    sortByArrivalTime(processes);
    showProcesses(processes);
    return 0;
}