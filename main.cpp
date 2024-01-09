#include "process.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <unistd.h>
#include <filesystem>
#include <algorithm>
#include <iomanip>

namespace fs = std::filesystem;

int getProcesses(std::vector<ProcessInfo> &processes)
{
    int count = 0;
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
                // - Procesos RT usan una escala de prioridad diferente (1 a 99).
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
                    count++;
                    break;
                }
                default: // Ignorar todo lo demas
                    break;
                }
            }
        }
    return count;
}

void showProcesses(const std::vector<ProcessInfo>& processes, int count)
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
        for (int i = 0; i < count && i < processes.size(); ++i) {
            const ProcessInfo& process = processes[i];
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

void showResults(const std::vector<ProcessInfo> &processes, int count)
{
    double TotalTiempoEspera = 0, TotalTiempoRespuesta = 0;  
    showProcesses(processes, count);

    for (int i = 0; i < count; i++)
    {
        TotalTiempoEspera += processes[i].waiting_time;
        TotalTiempoRespuesta += processes[i].response_time;
    }

    double PromTiempoEspera = TotalTiempoEspera / count;
    double PromTiempoRespuesta = TotalTiempoRespuesta / count;

    std::cout << "\nTiempo promedio de espera: " << PromTiempoEspera << std::endl;
    std::cout << "Tiempo promedio de respuesta: " << PromTiempoRespuesta << std::endl;
}

void SJF(std::vector<ProcessInfo> &processes, int n)
{
    double tiempo_total = 0;
    int procesos_restantes = n;

    for (int i = 0; i < n; ++i)
        processes[i].left_time = processes[i].burst_time;

    while (procesos_restantes > 0)
    {
        int shortest_job = -1;
        double menor_tiempo = std::numeric_limits<double>::max();
        int todos_procesos_terminados = 1;

        for (int i = 0; i < n; ++i)
            if (processes[i].arrival_time <= tiempo_total && processes[i].left_time > 0)
            {
                if (processes[i].burst_time < menor_tiempo)
                {
                    menor_tiempo = processes[i].burst_time;
                    shortest_job = i;
                }
                todos_procesos_terminados = 0;
            }

        if (todos_procesos_terminados)
            break;
        if (shortest_job == -1)
            tiempo_total++;
        else
        {
            processes[shortest_job].left_time = 0.0;
            processes[shortest_job].waiting_time = tiempo_total - processes[shortest_job].arrival_time;
            processes[shortest_job].response_time = processes[shortest_job].waiting_time + processes[shortest_job].burst_time;
            tiempo_total += processes[shortest_job].burst_time;
            procesos_restantes--;
        }
    }
}

void sortByArrivalTime(std::vector<ProcessInfo>& processes)
{
    std::sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        return a.arrival_time < b.arrival_time;
    });
}

void FCFS(std::vector<ProcessInfo> &lista_procesos, int n)
{
    int wait, espera = 0;
    if (lista_procesos[0].arrival_time == 0)
        lista_procesos[0].waiting_time = 0;
    else
    {
        lista_procesos[0].waiting_time = lista_procesos[0].arrival_time;
        espera = lista_procesos[0].arrival_time;
    }
    lista_procesos[0].response_time = lista_procesos[0].burst_time;
    for (int i = 1; i < n; i++)
    {
        int wait = espera;
        for (int j = 0; j < i; j++)
        {
            wait += lista_procesos[j].burst_time;
        }
        lista_procesos[i].waiting_time = wait - lista_procesos[i].arrival_time;
        if (lista_procesos[i].waiting_time < 0)
        {
            lista_procesos[i].waiting_time = 0;
        }
        lista_procesos[i].response_time = lista_procesos[i].burst_time + lista_procesos[i].waiting_time;
    }
}

int main()
{
    //ProcessInfo processes[2048];
    std::vector<ProcessInfo> processes;
    int count = getProcesses(processes);
    std::cout<<count<<std::endl;
    int opcion;

    while (opcion != 4)
    {
        std::cout << "\nSelecciona una opcion:\n";
        std::cout << "1. Mostrar procesos\n";
        std::cout << "2. Algoritmo SJF\n";
        std::cout << "3. Algoritmo FCFS\n";
        std::cout << "4. Salir\n";
        std::cout << "Opcion: ";
        std::cin >> opcion;

        if (opcion == 1)
        {
            showProcesses(processes, count);
        }
        else if (opcion == 2)
        {
            SJF(processes, count);
            showResults(processes, count);
        }
        else if (opcion == 3) {
            FCFS(processes, count);
            showResults(processes, count);
        }
        else if (opcion == 5) {
            sortByArrivalTime(processes);
            showProcesses(processes, count);
        }
    }

    return 0;
}