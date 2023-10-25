project(Testbed VERSION 0.0.1 LANGUAGES CXX)

file(GLOB_RECURSE TESTBED_SRC_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.c??)
file(GLOB_RECURSE TESTBED_HEADER_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} include/*.h)

include_directories(include)

add_executable(testbed ${TESTBED_SRC_FILES} ${TESTBED_HEADER_FILES})

target_link_libraries(testbed engine)

if(MSVC)
    set_property(TARGET testbed PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()