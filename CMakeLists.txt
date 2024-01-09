cmake_minimum_required(VERSION 3.14)

project(VisualizadorProcesos)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Incluir directorios
include_directories(${PROJECT_SOURCE_DIR}/inc/)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/build")

find_package(httplib COMPONENTS OpenSSL ZLIB Brotli REQUIRED)
find_package(nlohmann_json 3.11.3 REQUIRED)

message("Building with CMake version: ${CMAKE_VERSION}")

# Agregar tus archivos fuente
add_executable(${PROJECT_NAME} src/process_utils.cpp src/main.cpp)

# Enlazar con las bibliotecas instaladas por Conan
target_link_libraries(${PROJECT_NAME} PRIVATE httplib::httplib nlohmann_json::nlohmann_json)

# Habilitar C++17
target_compile_features(VisualizadorProcesos PRIVATE cxx_std_17)
