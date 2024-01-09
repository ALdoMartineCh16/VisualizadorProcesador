#include <nlohmann/json.hpp>
#include <httplib.h>
#include "process_utils.hpp"

using json = nlohmann::json;

int main()
{
    httplib::Server svr;

    // Configurar los encabezados CORS
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},  // Permitir cualquier origen
        {"Access-Control-Allow-Methods", "GET, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    std::vector<ProcessInfo> processInfoList;
    
    // Ruta para la solicitud inicial de información de procesos
    svr.Get("/initial_process_info", [&processInfoList](const httplib::Request& req, httplib::Response& res) {
        // Llamar a la función getProcesses y ordenar por tiempo de llegada
        getProcesses(processInfoList);
        sortByArrivalTime(processInfoList);

        // Convertir la lista de información de procesos a JSON
        nlohmann::json jsonData;
        for (const auto& processInfo : processInfoList) {
            jsonData.push_back({
                {"name", processInfo.name},
                {"pid", processInfo.pid},
                {"priority", processInfo.priority},
                {"burst_time", processInfo.burst_time},
                {"arrival_time", processInfo.arrival_time},
                {"left_time", processInfo.left_time},
                {"waiting_time", processInfo.waiting_time},
                {"response_time", processInfo.response_time}
            });
        }

        // Enviar la respuesta al cliente
        res.set_content(jsonData.dump(), "application/json");
    });

    // Ruta para la solicitud de información de procesos cuando se presiona el botón
    svr.Get("/update_process_info", [&processInfoList](const httplib::Request& req, httplib::Response& res) {
        // Limpiar la lista antes de realizar la actualización
        processInfoList.clear();

        // Llamar a la función getProcesses y ordenar por tiempo de llegada
        getProcesses(processInfoList);
        sortByArrivalTime(processInfoList);

        // Convertir la lista de información de procesos a JSON
        nlohmann::json jsonData;
        for (const auto& processInfo : processInfoList) {
            jsonData.push_back({
                {"name", processInfo.name},
                {"pid", processInfo.pid},
                {"priority", processInfo.priority},
                {"burst_time", processInfo.burst_time},
                {"arrival_time", processInfo.arrival_time},
                {"left_time", processInfo.left_time},
                {"waiting_time", processInfo.waiting_time},
                {"response_time", processInfo.response_time}
            });
        }

        // Enviar la respuesta al cliente
        res.set_content(jsonData.dump(), "application/json");
    });

    std::cout << "Servidor iniciado en http://localhost:8080" << std::endl;
    svr.listen("localhost", 8080);

    return 0;
}