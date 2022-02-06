cmake_minimum_required(VERSION 3.12)

project(APIWrapper)

include(../../CMake/CommonMacros.txt)

SETUP_APP(APIWrapper "APIWrapper")

target_link_libraries(APIWrapper glfw)
