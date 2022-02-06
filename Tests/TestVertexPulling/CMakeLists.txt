cmake_minimum_required(VERSION 3.12)

project(VertexPulling)

include(../../CMake/CommonMacros.txt)

SETUP_APP(VertexPulling "VertexPulling")

target_sources(VertexPulling PRIVATE ${CMAKE_SOURCE_DIR}/shared/internal/GLShader.cpp)

target_link_libraries(VertexPulling glad glfw assimp)
