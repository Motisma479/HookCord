cmake_minimum_required(VERSION 3.8)
set (CMAKE_CXX_STANDARD 20)
add_compile_definitions(CPP_TEMPLATE_VERSION="0.0.0") # Remember to also change it in the code and workflow

project(CPP_Template)

include_directories("Headers")

set(PRECOMPILED_HEADER "Headers/pch.hpp")
set(SOURCES
    "Sources/main.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_precompile_headers(${PROJECT_NAME} PRIVATE ${PRECOMPILED_HEADER})