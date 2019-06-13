cmake_minimum_required(VERSION 2.8.0)
project(heliumdb CXX)

# compile options
option(DEBUG "Enable debug build" OFF)
option(TESTS "Enable unittests" OFF)
set(PYTHON_VER "3" CACHE STRING "Python version to build with")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

add_compile_options(-Wall)
if (DEBUG)
  message(STATUS "heliumdb DEBUG: ON")
  add_compile_options(-g3)
else ()
  message(STATUS "heliumdb DEBUG: OFF")
  add_compile_options(-g -O3)
endif (DEBUG)

# allow external projects
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake)

# Find dependencies
find_package(PythonInterp ${PYTHON_VER} REQUIRED)
find_package(PythonLibs ${PYTHON_VER} REQUIRED)
include_directories(SYSTEM ${PYTHON_INCLUDE_PATH})

set(INCLUDE_PATHS
    ${PROJECT_SOURCE_DIR}/src/heliumdb/
    ${CMAKE_INSTALL_PREFIX}/include
)

SET(CMAKE_INSTALL_RPATH "/usr/lib")

configure_file(
  ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config-install.cmake
  @ONLY
  )

# add source
add_subdirectory(src)

if(TESTS)
  include(CTest)
  enable_testing()
  
  # add test dir if enabled
  add_subdirectory(test)
endif()