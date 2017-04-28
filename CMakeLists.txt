cmake_minimum_required (VERSION 3.8)

project (playground)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.hpp")
file(GLOB STAGING "staging/*.*")

add_executable(playground ${SOURCES} ${HEADERS} ${STAGING})
