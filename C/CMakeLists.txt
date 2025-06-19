cmake_minimum_required(VERSION 3.30)
project(jeu_Quoridor_GRP18 C)

set(CMAKE_C_STANDARD 11)

add_library(jeu_Quoridor_GRP18 STATIC quoridor.c
        plateau.c
        joueur.c
        joueur.h
        plateau.h
        quoridor.h
        main.c)

add_executable(Jeu_Quoridor main.c)
