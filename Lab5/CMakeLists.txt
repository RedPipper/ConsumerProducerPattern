cmake_minimum_required(VERSION 3.26)
project(Lab5)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lab5
        main.cpp
        tsQueue.h
        tsQueue.cpp
        listaParticipanti.h
        listaParticipanti.cpp
)
target_compile_options(Lab5 PRIVATE -O5 )

