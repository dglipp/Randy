cmake_minimum_required(VERSION 3.12)

project(ImGUI_test)

include(../../CMake/CommonMacros.txt)
include_directories(../../deps/src/imgui)

SETUP_APP(ImGUI_test "ImGUI_test")

target_link_libraries(ImGUI_test glad glfw)

