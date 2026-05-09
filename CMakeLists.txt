cmake_minimum_required(VERSION 4.1)
project(TrafficManagementSystem)

set(CMAKE_CXX_STANDARD 20)

# Include ASIO headers
include_directories("asio-1.36.0/asio-1.36.0/include")

# Add source files
add_executable(untitled1 SPF.cpp)

# Link Windows socket libraries (Winsock + Mswsock)
target_link_libraries(untitled1 PRIVATE ws2_32 mswsock)
configure_file(routes.txt routes.txt COPYONLY)
configure_file(index.html index.html COPYONLY)
configure_file(style.css style.css COPYONLY)
configure_file(bgpicture.jpg bgpicture.jpg COPYONLY)
configure_file(bgvideo.mp4 bgvideo.mp4 COPYONLY)
configure_file(opt1.html opt1.html COPYONLY)
configure_file(opt1.css opt1.css COPYONLY)
configure_file(opt2.html opt2.html COPYONLY)
configure_file(opt2.css opt2.css COPYONLY)
configure_file(opt3.html opt3.html COPYONLY)
configure_file(opt3.css opt3.css COPYONLY)
configure_file(opt4.html opt4.html COPYONLY)
configure_file(opt4.css opt4.css COPYONLY)
configure_file(opt5.html opt5.html COPYONLY)
configure_file(opt5.css opt5.css COPYONLY)
configure_file(opt6.html opt6.html COPYONLY)
configure_file(opt6.css opt6.css COPYONLY)
configure_file(opt7.html opt7.html COPYONLY)
configure_file(opt7.css opt7.css COPYONLY)
configure_file(opt8.html opt8.html COPYONLY)
configure_file(opt8.css opt8.css COPYONLY)