# Visualizador de Procesos

Este proyecto es un visualizador de algoritmos de planificacion de procesos del OS (FIFO, SJF, RR) 

## Prerrequisitos

Asegúrate de tener instalados los siguientes componentes antes de construir el proyecto:

- [CMake](https://cmake.org/)

    ```bash
    sudo apt-get update
    sudo apt-get install cmake
    ```

- [Conan](https://conan.io/)

    ```bash
    pip install conan
    ```

- [build-essential para GCC/G++](https://linuxconfig.org/how-to-install-g-the-c-compiler-on-ubuntu-20-04-lts-focal-fossa-linux):

    ```bash
    sudo apt-get install build-essential
    ```

## Construcción del Proyecto

1. Clona el repositorio:

    ```bash
    git clone https://github.com/ALdoMartineCh16/VisualizadorProcesador.git
    cd VisualizadorProcesos
    ```

2. Instala las dependencias utilizando Conan:

    ```bash
    conan install . --output-folder=build --build=missing
    ```

3. Cambia al directorio de construcción:

    ```bash
    cd build
    ```

4. Configura el proyecto con CMake:

    ```bash
    cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
    ```

5. Compila el proyecto:

    ```bash
    cmake --build .
    ```

6. Ejecuta el visualizador de procesos:

    ```bash
    ./VisualizadorProcesos
    ```
7. Abre en tu navegador el [archivo HTML](VisualizadorProcesador/blob/dev/frontend/visualizer.html):

## Notas

- Este proyecto ha sido probado en Linux. La compatibilidad con otros sistemas operativos no ha sido verificada.
- Se recomienda el uso de VSCode con la extensión de CMake y CMake Tools para un entorno de desarrollo más fácil.

