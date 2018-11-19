cmake_minimum_required(VERSION 3.12)
project(socket)

set(CMAKE_CXX_STANDARD 17)

# add_executable(socket server.cpp client.cpp udp_receiver.c udp_sender.c)

add_executable(socket udp_receiver.c)