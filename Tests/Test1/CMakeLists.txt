cmake_minimum_required(VERSION 3.12)

project(Test1)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Test1 "Test1")

target_link_libraries(Test1 glad glfw)

