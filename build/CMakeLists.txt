cmake_minimum_required(VERSION 2.6)
project(TheGame)

set(ROOT_DIR ${PROJECT_SOURCE_DIR}/..)
set(EXECUTABLE_OUTPUT_PATH ${ROOT_DIR}/bin)

include_directories(${ROOT_DIR}/src)
file(GLOB_RECURSE SOURCES ${ROOT_DIR}/src/*.cpp)

add_executable(game ${SOURCES})

