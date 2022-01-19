cmake_minimum_required(VERSION 3.12)

project(Etc2compTest)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Etc2compTest "Etc2compTest")

target_sources(Etc2compTest PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../../../deps/src/etc2comp/EtcTool/EtcFile.cpp)
target_sources(Etc2compTest PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../../../deps/src/etc2comp/EtcTool/EtcFileHeader.cpp)

target_link_libraries(Etc2compTest glad glfw assimp EtcLib)

