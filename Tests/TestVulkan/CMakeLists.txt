cmake_minimum_required(VERSION 3.12)

project(FirstVulkan)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)
include_directories(../../deps/src/glslang/include)

SETUP_APP(FirstVulkan "FirstVulkan")

target_link_libraries(FirstVulkan PRIVATE glfw volk glslang SPIRV assimp SharedUtils)
