cmake_minimum_required(VERSION 3.10)
project(mpiapptimer VERSION 1.0 DESCRIPTION "Dynamically Loaded Timer library for MPI Applications" LANGUAGES CXX Fortran)

find_package(MPI)
if(NOT MPI_FOUND)
    message(WARNING "MPI not found. mpisee will not compile.")
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g -Wall -march=native")


add_library(mpiapptimer SHARED src/mpiapptimer.cpp)

target_include_directories(mpiapptimer PRIVATE ${MPI_CXX_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
target_link_libraries(mpiapptimer PRIVATE ${MPI_CXX_LIBRARIES})
set_target_properties(mpiapptimer PROPERTIES SOVERSION 1)
