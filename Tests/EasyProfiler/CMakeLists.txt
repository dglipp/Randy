cmake_minimum_required(VERSION 3.12)

project(EasyProfilerTest)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/easy_profiler/include)

add_definitions(-DBUILD_WITH_EASY_PROFILER=1)

SETUP_APP(EasyProfilerTest "EasyProfilerTest")

target_link_libraries(EasyProfilerTest glad glfw easy_profiler)