cmake_minimum_required(VERSION 4.1)
project(Turbomachines_solver)

set(CMAKE_CXX_STANDARD 20)

add_executable(Turbomachines_solver main.cpp
        Coordinate_Generation_engine.cpp
        Coordinate_Generation_engine.h
        Frontend/src/main.cpp
        Frontend/src/ui/MainWindow.cpp
        Frontend/src/ui/MainWindow.h
        Frontend/src/ui/VelocityTriangleWidget.cpp
        Frontend/src/ui/VelocityTriangleWidget.h)

