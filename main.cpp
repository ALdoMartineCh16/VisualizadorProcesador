#include "process.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include "vector"
#include <dirent.h>
#include <limits.h>
#include <sstream>
#include <string>
using  namespace  std;
int getProcesses(vector<ProcessInfo> &processes)
{
    DIR *dir;
    struct dirent *entry;
    FILE *fp;
    char path[128];
    int count = 0;
    dir = opendir("/proc");

    if (dir == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (isdigit(*entry->d_name))
        {
            int pid = atoi(entry->d_name);
            sprintf(path, "/proc/%d/stat", pid);
            fp = fopen(path, "r");

            if (fp != NULL)
            {
                ProcessInfo info;
                // Modificación: Leer el 22º dígito en info.arrival_time

                fscanf(fp, "%d %s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %d",&info.pid, info.name, &info.priority);
                fclose(fp);

                std::ifstream file(path);

                if (!file.is_open()) {
                    std::cerr << "Error: No se puede abrir el archivo " << path << std::endl;
                    return -1;
                }

                // Leer el contenido del archivo stat
                std::string line;
                std::getline(file, line);
                file.close();

                // Extraer el tiempo de inicio (starttime) del archivo stat
                std::istringstream iss(line);
                string word;
                for (int i = 0; i <= 21; ++i) {
                    iss >> word;
                }
                int intValue = std::stoi(word);
                info.arrival_time = intValue;

                sprintf(path, "/proc/%d/sched", pid);
                fp = fopen(path, "r");

                if (fp != NULL)
                {
                    char line[256];

                    while (fgets(line, sizeof(line), fp))
                    {
                        if (strstr(line, "se.sum_exec_runtime"))
                            sscanf(line, "se.sum_exec_runtime %*s %d", &info.burst_time);
                    }

                    fclose(fp);
                    info.left_time = info.burst_time;
                    processes.push_back(info);
                    count++;
                }
            }
        }
    }

    closedir(dir);
    return count;
}
void showProcesses(vector<ProcessInfo> &processes, int count)
{
    cout << "PID\tName\t\t\t\t\tPrioridad\t\tTiempo de Rafaga\t\tTiempo de llegada\t\tTiempo Restante\n";

    for (int i = 0; i < count; i++)
    {
        char adjustedName[257];
        strncpy(adjustedName, processes[i].name, 256);
        adjustedName[256] = '\0';

        cout<<processes[i].pid<<"\t"<<adjustedName<<"\t\t\t\t\t"<<processes[i].priority<<"\t\t"<<processes[i].burst_time<<"\t\t"<<processes[i].arrival_time<< "\t\t" << processes[i].left_time << endl;
    }
}

void showResults(vector<ProcessInfo> &processes, int count)
{

    double TotalTiempoEspera = 0;
    double TotalTiempoRespuesta = 0;

    std::cout << "PID\tName\t\t\t\t\tPrioridad\t\tTiempo de Rafaga\t\tTiempo de llegada\t\tTiempo de espera\t\tTiempo de respuesta\n";

    for (int i = 0; i < count; i++)
    {
        char adjustedName[257];
        strncpy(adjustedName, processes[i].name, 256);
        adjustedName[256] = '\0';

        std::cout << processes[i].pid << "\t" << adjustedName << "\t\t\t\t\t" << processes[i].priority
                  << "\t\t" << processes[i].burst_time << "\t\t" << processes[i].arrival_time
                  << "\t\t" << processes[i].waiting_time << "\t\t" << processes[i].response_time << std::endl;

        TotalTiempoEspera += processes[i].waiting_time;
        TotalTiempoRespuesta += processes[i].response_time;
    }

    double PromTiempoEspera = TotalTiempoEspera / count;
    double PromTiempoRespuesta = TotalTiempoRespuesta / count;

    cout << "\nTiempo promedio de espera: " << PromTiempoEspera << endl;
    cout << "Tiempo promedio de respuesta: " << PromTiempoRespuesta << endl;
}
void SJF(vector<ProcessInfo> &lista_procesos, int n)
{
    int tiempo_total = 0;
    int procesos_restantes = n;

    for (int i = 0; i < n; ++i)
        lista_procesos[i].left_time = lista_procesos[i].burst_time;

    while (procesos_restantes > 0)
    {
        int shortest_job = -1;
        int menor_tiempo = INT_MAX;
        int todos_procesos_terminados = 1;

        for (int i = 0; i < n; ++i)
        {
            if (lista_procesos[i].arrival_time <= tiempo_total && lista_procesos[i].left_time > 0)
            {
                if (lista_procesos[i].burst_time < menor_tiempo)
                {
                    menor_tiempo = lista_procesos[i].burst_time;
                    shortest_job = i;
                }

                todos_procesos_terminados = 0;
            }
        }

        if (todos_procesos_terminados)
            break;

        if (shortest_job == -1)
            tiempo_total++;
        else
        {
            lista_procesos[shortest_job].left_time = 0;
            lista_procesos[shortest_job].waiting_time = tiempo_total - lista_procesos[shortest_job].arrival_time;
            lista_procesos[shortest_job].response_time = lista_procesos[shortest_job].waiting_time + lista_procesos[shortest_job].burst_time;
            tiempo_total += lista_procesos[shortest_job].burst_time;
            procesos_restantes--;
        }
    }
    showResults(lista_procesos, n);
}

void SortTimeArrival(ProcessInfo *lista_procesos, int n)
{
    for (int i=0; i < n-1; i++){
        for (int j=0; j<n-i-1; j++){
            if (lista_procesos[j].arrival_time > lista_procesos[j + 1].arrival_time){
                ProcessInfo temp = lista_procesos[j];
                lista_procesos[j] = lista_procesos[j + 1];
                lista_procesos[j + 1] = temp;
            }
        }
    }
}

void FCFS(vector<ProcessInfo> &lista_procesos, int n)
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
    vector<ProcessInfo> processes;
    int count = getProcesses(processes);
    cout<<count<<endl;
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
        }else if (opcion == 3) {
            FCFS(processes, count);
            showResults(processes, count);
        }
    }

    return 0;
}