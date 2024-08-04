# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# Project's name
project(evlearn)

# Set the C standard
set(CMAKE_C_STANDARD 11)

set(HEADERS "include/evlearn.h")
set(SOURCES "src/evlearn.c")

# Add executable target with source files listed in SOURCE_FILES variable
add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${HEADERS})