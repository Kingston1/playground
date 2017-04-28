cmake_minimum_required (VERSION 3.8)

project (playground)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

add_executable(playground ${SOURCES} ${HEADERS})
