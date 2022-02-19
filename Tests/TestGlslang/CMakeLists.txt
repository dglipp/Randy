cmake_minimum_required(VERSION 3.12)

project(GlslangCompiling)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)
include_directories(../../deps/src/glslang/include)

SETUP_APP(GlslangCompiling "GlslangCompiling")

target_sources(GlslangCompiling PRIVATE ${CMAKE_SOURCE_DIR}/shared/internal/Bitmap.cpp)
target_sources(GlslangCompiling PRIVATE ${CMAKE_SOURCE_DIR}/shared/internal/GLShader.cpp)
target_sources(GlslangCompiling PRIVATE ${CMAKE_SOURCE_DIR}/shared/internal/UtilsVulkan.cpp)

target_link_libraries(GlslangCompiling PRIVATE glfw glad volk SPIRV glslang SharedUtils)