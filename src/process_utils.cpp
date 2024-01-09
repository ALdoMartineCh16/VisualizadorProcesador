#include "../inc/process_utils.hpp"

void getProcesses(std::vector<ProcessInfo> &processes)
{
    // Para recorrer el contenido del directorio 'proc'
    for (const auto &entry : fs::directory_iterator("/proc"))
        // Filtrar carpetas que sean digitos (procesos)
        if (isdigit(entry.path().filename().string()[0]))
        {
            std::ifstream statFile(entry.path() / "stat");
            // Abrir statFile del proceso
            if (statFile.is_open())
            {
                // Leer el contenido del archivo stat
                std::string line;
                std::getline(statFile, line);
                statFile.close();
                std::istringstream ss(line);
                std::vector<std::string> statValues;
                // Fill
                std::string value, name;
                ss >> value;
                statValues.push_back(value);
                // Name
                ss >> value;
                name += value;
                while (name.back() != ')') 
                {
                    name += " ";
                    ss >> value;
                    name += value;
                }
                // Suprimir parentesis
                if (!name.empty() && name.front() == '(' && name.back() == ')')
                    name = name.substr(1, name.size() - 2);
                statValues.push_back(name);
                // Rest of values
                while (ss >> value) statValues.push_back(value);
                // IMPORTANTE: Excluir procesos RT (real-time) / kernel-space
                // https://man7.org/linux/man-pages/man2/sched_setscheduler.2.html
                // - Procesos RT usan una escala de prioridad diferente (0 a 99).
                // - Se distinguen por el planificador que esten usando.
                // - https://man7.org/linux/man-pages/man7/sched.7.html
                // - Estos procesos son una minoria en comparacion a los de user-space.
                switch (std::stoi(statValues.at(40)))
                {
                case 0: case 3: case 5: // SCHED_OTHER/SCHED_NORMAL, SCHED_IDLE, SCHED_BATCH
                {
                    ProcessInfo info;
                    // PID
                    info.pid = std::stoi(statValues.at(0));
                    
                    // Nombre
                    info.name = statValues.at(1);

                    // Priority/Niceness
                    info.priority = std::stoi(statValues.at(18));

                    // Burst Time/CPU Time (nano)
                    std::ifstream schedFile(entry.path() / "sched");
                    std::string l, bt; 
                    while (std::getline(schedFile, l))
                        if (l.find("se.sum_exec_runtime") != std::string::npos)
                        {
                            std::istringstream iss(l);
                            std::string token;
                            // revisar en ejecucion
                            iss >> token >> token >> bt;
                        }
                    schedFile.close();
                    info.burst_time = std::stod(bt);
                    // Procesos en espera (sin tiempo de rafaga, no se consideran)
                    if (info.burst_time == 0.0) continue;

                    // Arrival Time (clock ticks -> sec)
                    info.arrival_time = (std::stod(statValues.at(21))) / sysconf(_SC_CLK_TCK);

                    // Left time
                    info.left_time = info.burst_time;
                    // Resto
                    info.response_time = info.waiting_time = 0.0;
                    processes.push_back(info);
                    break;
                }
                default: // Ignorar todo lo demas
                    break;
                }
            }
        }
    std::cout << "Nº processes: " << processes.size() << std::endl;
}

void showProcesses(const std::vector<ProcessInfo>& processes)
{
    // Imprimir encabezado de la tabla
    std::cout << std::left << std::setw(40) << "Name"
            << std::setw(7) << "PID"
            << std::setw(10) << "Priority"
            << std::setw(16) << "Burst Time"
            << std::setw(18) << "Arrival Time"
            << std::setw(15) << "Left Time"
            << std::setw(18) << "Waiting Time"
            << std::setw(19) << "Response Time"
            << std::endl;

    // Imprimir cada proceso
    for (const auto& process : processes) 
    {
        std::cout << std::left << std::setw(40) << process.name
                << std::setw(7) << process.pid
                << std::setw(10) << process.priority
                << std::setw(16) << process.burst_time
                << std::setw(18) << process.arrival_time
                << std::setw(15) << process.left_time
                << std::setw(18) << process.waiting_time
                << std::setw(19) << process.response_time
                << std::endl;
    }
}

void sortByArrivalTime(std::vector<ProcessInfo>& processes)
{
    std::sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        return a.arrival_time < b.arrival_time;
    });
}