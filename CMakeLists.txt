cmake_minimum_required(VERSION 3.10)
project(Randy CXX)

add_subdirectory(engine)
add_subdirectory(testbed)

if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()