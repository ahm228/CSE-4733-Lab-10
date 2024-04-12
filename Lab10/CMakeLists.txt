cmake_minimum_required(VERSION 3.12)
project(OperatingSystemsAssignment)

# Set C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

# Add the main executable
add_executable(OperatingSystemsAssignment src/main.cpp)

target_link_libraries(${PROJECT_NAME} Threads::Threads)

