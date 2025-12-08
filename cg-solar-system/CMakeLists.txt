##############################################################################

cmake_minimum_required(VERSION 3.5)
project(SolarSystem)

##############################################################################
# paths
##############################################################################


set(PROJECT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set(CMAKE_PROJECT_VERSION 2.0.0)
set(CMAKE_PROJECT_VERSION_MAJOR 2)
set(CMAKE_PROJECT_VERSION_MINOR 0)
set(CMAKE_PROJECT_VERSION_PATCH 0)

##############################################################################
# dependencies
##############################################################################

cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)

##############################################################################
# compiler flags
##############################################################################

# we need C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#our base path for shaders and textures
add_definitions(-DBASE_PATH="${CMAKE_SOURCE_DIR}/")

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_USE_MATH_DEFINES -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS")
  add_compile_options(/wd4267) # conversion from size_t
  add_compile_options(/wd4244) # conversion from double to float
  add_compile_options(/wd4305) # truncation from double to float
  add_compile_options(/wd4005) # macro redefinition
endif()

if (EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -s WASM=1 --no-heap-copy -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s MINIFY_HTML=0 -s DISABLE_EXCEPTION_CATCHING=0")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

##############################################################################
# build type
##############################################################################

# default to Release builds
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
endif()


##############################################################################
# STB 
# (place *before* GLFW since GLFW has an old copy of stb_image_write.h)
##############################################################################

include_directories(${CMAKE_SOURCE_DIR}/external/stb_image/)
add_subdirectory(external/stb_image)

##############################################################################
# GLFW & GLEW
##############################################################################

# setup use of GLFW and GLEW
if (NOT EMSCRIPTEN)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
  set(GLFW_INSTALL OFF CACHE BOOL "")

  include_directories(${CMAKE_SOURCE_DIR}/external/glew/include/)
  add_definitions(-DGLEW_STATIC)
  add_subdirectory(external/glew)

  include_directories(${CMAKE_SOURCE_DIR}/external/glfw/include/)
  add_subdirectory(external/glfw)
else()
  # GLEW and GLFW are built-in
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3")
endif()

##############################################################################
# source code directories
##############################################################################


# add source directory to include path
include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${CMAKE_SOURCE_DIR}/external/utils/)
include_directories(${CMAKE_SOURCE_DIR}/src/shader/)

# build source directory
add_subdirectory(external/utils)
add_subdirectory(src)


##############################################################################

