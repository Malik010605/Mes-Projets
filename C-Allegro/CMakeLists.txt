cmake_minimum_required(VERSION 3.29)
project(projet_badland C)

set(CMAKE_C_STANDARD 11)

add_executable(projet_badland main.c
        personnage.h
        personnage.c)

target_link_libraries(projet_badland -lmingw32 -lalleg44)