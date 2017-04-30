cmake_minimum_required (VERSION 3.6)

project (playground)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.hpp")
file(GLOB STAGING "staging/*.*")

add_executable(playground ${SOURCES} ${HEADERS} ${STAGING})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT playground)

option(USE_CLANG "build application with clang" OFF) # OFF is the default
if(USE_CLANG)
    SET (CMAKE_C_FLAGS_INIT                "-Wall")
    SET (CMAKE_C_FLAGS_DEBUG_INIT          "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELEASE_INIT        "-O3 -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

    SET (CMAKE_CXX_FLAGS_INIT                "-Wall -std=c++14 -stdlib=libc++")
    SET (CMAKE_CXX_FLAGS_DEBUG_INIT          "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE_INIT        "-O3 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")
    SET (CMAKE_EXE_LINKER_FLAGS_INIT         "")
endif(USE_CLANG)
