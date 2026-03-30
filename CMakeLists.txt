cmake_minimum_required(VERSION 4.2)
project(ContactManagementSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ContactManagementSystem main.cpp
        menu.cpp
        menu.h
        model.h
        tool.cpp
        tool.h
        global.cpp
        global.h
        service.h
        service.cpp
        account_service.cpp
        account_service.h
        account_file.cpp
        account_file.h
        tracking_file.cpp
        tracking_file.h
        tracking_service.cpp
        tracking_service.h
        points_file.cpp
        points_file.h
        points_service.cpp
        points_service.h
        large_model.cpp
        large_model.h)
