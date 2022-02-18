cmake_minimum_required(VERSION 3.12)

project(Cubemap)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Cubemap "Cubemap")

target_sources(Cubemap PRIVATE ${CMAKE_SOURCE_DIR}/shared/internal/GLShader.cpp)
target_sources(Cubemap PRIVATE ${CMAKE_SOURCE_DIR}/shared/internal/Bitmap.cpp)

target_link_libraries(Cubemap glad glfw assimp)
