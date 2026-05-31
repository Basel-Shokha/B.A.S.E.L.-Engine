cmake_minimum_required(VERSION 3.22)
project(B_A_S_E_L__Engine_main)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories("External Libraries")

add_executable(B_A_S_E_L__Engine_main
        "External Libraries/json.hpp"
        EVRoutePlanner.cpp
        EVRoutePlanner.h
        FleetDispatcher.cpp
        FleetDispatcher.h
        FlowNetwork.cpp
        FlowNetwork.h
        Graph.cpp
        Graph.h
        main.cpp
        MapLoader.cpp
        MapLoader.h
        MinHeap.cpp
        MinHeap.h
        SwarmController.cpp
        SwarmController.h
        Utilities.cpp
        Utilities.h)
