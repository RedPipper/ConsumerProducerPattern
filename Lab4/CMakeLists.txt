cmake_minimum_required(VERSION 3.26)
project(Lab4)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lab4
        tsQueue.h
        tsQueue.cpp
        main.cpp
        listaParticipanti.h)

target_compile_options(Lab4 PRIVATE -O5)
