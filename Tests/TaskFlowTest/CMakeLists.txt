cmake_minimum_required(VERSION 3.12)

project(TaskFlowTest)

include(../../CMake/CommonMacros.txt)

SETUP_APP(TaskFlowTest "TaskFlowTest")

target_link_libraries(TaskFlowTest glad glfw)

