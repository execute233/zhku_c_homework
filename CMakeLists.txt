cmake_minimum_required(VERSION 4.0)

project(fuck_zhku_homework LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(lib)
include_directories(proj)

set(LIB_SOURCES
    lib/array_list.c
)

set(PROJ_SOURCES
    proj/data_restriction.c
    proj/data.c
    proj/io.c
    proj/console.c
    proj/sort.c
    proj/user_management.c
)

add_executable(fuck_zhku_homework 
    main.c 
    ${LIB_SOURCES} 
    ${PROJ_SOURCES}
)