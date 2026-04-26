cmake_minimum_required(VERSION 4.1)
project(Turbomachines_solver)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Core 3DCore 3DRender 3DExtras)

add_executable(Turbomachines_GUI
        Frontend/src/main.cpp
        Frontend/src/ui/MainWindow.cpp
        Frontend/src/ui/MainWindow.h
        Frontend/src/ui/VelocityTriangleWidget.cpp
        Frontend/src/ui/VelocityTriangleWidget.h
        Frontend/src/ui/ChatMentorWidget.cpp
        Frontend/src/ui/ChatMentorWidget.h
        Frontend/src/ui/ProfileExplorer3DWidget.cpp
        Frontend/src/ui/ProfileExplorer3DWidget.h
        Frontend/src/ui/ProfessorDashboard.cpp
        Frontend/src/ui/ProfessorDashboard.h
        Frontend/src/ui/SidebarWidget.cpp
        Frontend/src/ui/SidebarWidget.h
        Frontend/resources.qrc
)

target_link_libraries(Turbomachines_GUI PRIVATE
        Qt6::Widgets
        Qt6::Network
        Qt6::Core
        Qt6::3DCore
        Qt6::3DRender
        Qt6::3DExtras
)

set_target_properties(Turbomachines_GUI PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_executable(Turbomachine_CLI
        main.cpp
)

target_link_libraries(Turbomachine_CLI PRIVATE
        Qt6::Widgets
        Qt6::Network
        Qt6::Core
)

set_target_properties(Turbomachine_CLI PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

