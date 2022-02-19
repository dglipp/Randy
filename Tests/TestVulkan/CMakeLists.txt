cmake_minimum_required(VERSION 3.12)

project(FirstVulkan)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)
include_directories(../../deps/src/glslang/include)

SETUP_APP(FirstVulkan "GlslangCompiling")

target_sources(FirstVulkan PRIVATE ${CMAKE_SOURCE_DIR}/shared/internal/Bitmap.cpp)
target_sources(FirstVulkan PRIVATE ${CMAKE_SOURCE_DIR}/shared/internal/GLShader.cpp)
target_sources(FirstVulkan PRIVATE ${CMAKE_SOURCE_DIR}/shared/internal/UtilsVulkan.cpp)
target_sources(FirstVulkan PRIVATE ${CMAKE_SOURCE_DIR}/deps/src/glslang/StandAlone/ResourceLimits.cpp)

target_link_libraries(FirstVulkan PRIVATE glfw glad volk SPIRV glslang SharedUtils)