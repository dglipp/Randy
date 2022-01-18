cmake_minimum_required(VERSION 3.12)

project(Test1)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Texture "Test1")

target_link_libraries(Texture glad glfw)

