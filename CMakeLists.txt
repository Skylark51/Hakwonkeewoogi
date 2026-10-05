cmake_minimum_required(VERSION 3.24)

project(HakwonKeewoogi VERSION 0.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(HakwonKeewoogi
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/GameSimulation.cpp
    src/GameSimulation.h
    src/AcademySceneWidget.cpp
    src/AcademySceneWidget.h
)

target_link_libraries(HakwonKeewoogi PRIVATE Qt6::Widgets)

if(WIN32)
    set_target_properties(HakwonKeewoogi PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
