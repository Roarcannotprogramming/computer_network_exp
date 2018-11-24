cmake_minimum_required(VERSION 3.12)
project(project)

set(CMAKE_CXX_STANDARD 17)

add_executable(project rdt_gbn_receiver.c rdt_pkt_util.c)