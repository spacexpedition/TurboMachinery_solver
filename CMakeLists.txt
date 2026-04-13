cmake_minimum_required(VERSION 4.1)
project(Turbomachines_solver)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Core)

add_executable(Turbomachines_solver
        main.cpp
        #Frontend/src/main.cpp
        Frontend/src/ui/MainWindow.cpp
        Frontend/src/ui/MainWindow.h
        Frontend/src/ui/VelocityTriangleWidget.cpp
        Frontend/src/ui/VelocityTriangleWidget.h
)

target_link_libraries(Turbomachines_solver PRIVATE
        Qt6::Widgets
        Qt6::Network
        Qt6::Core
)

set_target_properties(Turbomachines_solver PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

