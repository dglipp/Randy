cmake_minimum_required(VERSION 3.12)

project(AssimpTest)

include(../../CMake/CommonMacros.txt)

SETUP_APP(AssimpTest "AssimpTest")

target_link_libraries(AssimpTest glad glfw assimp)

