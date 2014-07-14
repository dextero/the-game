cmake_minimum_required(VERSION 2.6)
project(TheGame)

set(ROOT_DIR ${PROJECT_SOURCE_DIR}/..)
set(EXECUTABLE_OUTPUT_PATH ${ROOT_DIR}/bin)

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DGLM_FORCE_RADIANS)

# libraires
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS}
                    ${GLEW_INCLUDE_DIRS})
set(LIBS ${OPENGL_LIBRARIES}
         ${GLEW_LIBRARIES})

# platform-specific
if(WIN32)
    add_definitions(-DPLATFORM_WIN32)
elseif(UNIX)
    add_definitions(-DPLATFORM_LINUX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include_directories(${ROOT_DIR}/src)
file(GLOB_RECURSE SOURCES ${ROOT_DIR}/src/*.cpp)

add_executable(game ${SOURCES})
target_link_libraries(game ${LIBS})

