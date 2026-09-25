cmake_minimum_required(VERSION 3.10)
project(PLMLite VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(plm_lite
    src/main.cpp
    src/Part.cpp
    src/Assembly.cpp
    src/BOMManager.cpp
)
