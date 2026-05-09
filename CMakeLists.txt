cmake_minimum_required(VERSION 4.1)
project(untitled1)

set(CMAKE_CXX_STANDARD 20)

# Include ASIO headers
include_directories("asio-1.36.0/asio-1.36.0/include")

# Add source files
add_executable(untitled1 SPF.cpp
        SPF.cpp)

# Link Windows socket libraries (Winsock + Mswsock)
target_link_libraries(untitled1 PRIVATE ws2_32 mswsock)
configure_file(routes.txt routes.txt COPYONLY)
