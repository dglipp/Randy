cmake_minimum_required(VERSION 3.12)

project(MeshOptimizerTest)

include(../../CMake/CommonMacros.txt)

SETUP_APP(MeshOptimizerTest "MeshOptimizerTest")

target_link_libraries(MeshOptimizerTest glad glfw assimp meshoptimizer)

