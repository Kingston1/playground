cmake_minimum_required(VERSION 3.6)

project(playground)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.hpp")
file(GLOB STAGING "staging/*.*")

add_executable(playground ${SOURCES} ${HEADERS} ${STAGING})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT playground)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
    message(STATUS "Configuring for Linux")
endif()

if(LINUX)
    add_definitions("-std=c++14")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        target_link_libraries(playground pthread)
    endif()
endif()
