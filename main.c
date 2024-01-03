#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <limits.h>


int getProcesses(struct ProcessInfo *processes)
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
                struct ProcessInfo info;
                fscanf(fp, "%d %s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %d",&info.pid, info.name, &info.priority);
                fclose(fp);
                sprintf(path, "/proc/%d/sched", pid);
                fp = fopen(path, "r");
                if (fp != NULL)
                {
                    char line[256];
                    while (fgets(line, sizeof(line), fp))
                    {
                        if (strstr(line, "se.sum_exec_runtime"))
                            sscanf(line, "se.sum_exec_runtime %*s %d", &info.burst_time);
                        else if (strstr(line, "se.statistics.wait_start"))
                            sscanf(line, "se.statistics.wait_start %*s %d", &info.arrival_time);
                    }
                    fclose(fp);
                    info.left_time = info.burst_time; // Asignar left_time igual a burst_time
                    processes[count] = info;
                    count++;
                }
            }
        }
    }
    closedir(dir);
    return count;
}
void showProcesses(struct ProcessInfo *processes, int count)
{
    printf("PID\tName\t\t\t\t\tPrioridad\t\tTiempo de Rafaga\t\tTiempo de llegada\t\tTiempo Restante\n");
    for (int i = 0; i < count; i++)
    {
        char adjustedName[257];
        strncpy(adjustedName, processes[i].name, 256);
        adjustedName[256] = '\0';
        printf("%d\t%s\t\t\t\t\t%d\t\t%d\t\t%d\t\t%d\n", processes[i].pid, adjustedName,
               processes[i].priority, processes[i].burst_time, processes[i].arrival_time, processes[i].left_time);
    }
}
void SJF(struct ProcessInfo *lista_procesos, int n)
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
}
void showResults(struct ProcessInfo *processes, int count)
{
    double TotalTiempoEspera = 0;
    double TotalTiempoRespuesta = 0;
    printf("PID\tName\t\t\t\t\tPrioridad\t\tTiempo de Rafaga\t\tTiempo de llegada\t\tTiempo de espera\t\tTiempo de respuesta\n");

    for (int i = 0; i < count; i++)
    {
        char adjustedName[257];
        strncpy(adjustedName, processes[i].name, 256);
        adjustedName[256] = '\0';
        printf("%d\t%s\t\t\t\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", processes[i].pid, adjustedName,
               processes[i].priority, processes[i].burst_time, processes[i].arrival_time,
               processes[i].waiting_time, processes[i].response_time);

        TotalTiempoEspera += processes[i].waiting_time;
        TotalTiempoRespuesta += processes[i].response_time;
    }

    double PromTiempoEspera = TotalTiempoEspera / count;
    double PromTiempoRespuesta = TotalTiempoRespuesta / count;

    printf("\nTiempo promedio de espera: %.2lf\n", PromTiempoEspera);
    printf("Tiempo promedio de respuesta: %.2lf\n", PromTiempoRespuesta);
}

int main()
{
    struct ProcessInfo processes[2048];
    int count = getProcesses(processes);
    int opcion;

    while(opcion!=3){
        printf("\nSelecciona una opcion:\n");
        printf("1. Mostrar procesos\n");
        printf("2. Algoritmo SJF\n");
        printf("3. Salir\n");
        printf("Opcion:");
        scanf("%d", &opcion);
        if(opcion ==1){
            showProcesses(processes, count);
        }else if(opcion == 2){
            SJF(processes, count);
            showResults(processes, count);
        }
    }

    return 0;
}
