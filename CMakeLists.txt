
cmake_minimum_required(VERSION 3.0)
project(BlockTree)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O9")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")


include_directories(includes)
include_directories(external)
#include_directories(external/sdsl-lite/include)

include(FetchContent)


FetchContent_Declare(
  divsufsort
  GIT_REPOSITORY https://github.com/simongog/libdivsufsort.git
  GIT_TAG        2.0.1
)

FetchContent_Declare(
  sdsl
  GIT_REPOSITORY https://github.com/elarielcl/sdsl-lite.git
  # GIT_REPOSITORY https://github.com/simongog/sdsl-lite
  # GIT_TAG        v2.1.1
)
# FetchContent_Declare(
#   sdsl
#   GIT_REPOSITORY https://github.com/simongog/sdsl-lite
#   GIT_TAG        v2.1.1
# )

FetchContent_GetProperties(sdsl)
if(NOT sdsl_POPULATED)
  FetchContent_Populate(sdsl)

  set(GENERATE_DOC OFF CACHE BOOL "Do not generate doxygen for sdsl-lite")
  
  add_subdirectory(${sdsl_SOURCE_DIR} ${sdsl_BINARY_DIR})# EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(divsufsort)
if(NOT divsufsort_POPULATED)
  FetchContent_Populate(divsufsort)

  set(BUILD_SHARED_LIBS OFF CACHE BOOL "Do not build a shared library for libdivsufsort")
  set(BUILD_EXAMPLES OFF CACHE BOOL "Do not build libdivsufsort example")
  set(BUILD_DIVSUFSORT64 ON CACHE BOOL "Build libdivsufsort in 64-bits mode")

  add_subdirectory(${divsufsort_SOURCE_DIR} ${divsufsort_BINARY_DIR})# EXCLUDE_FROM_ALL)

  target_include_directories(divsufsort PUBLIC "${divsufsort_BINARY_DIR}/include")
  target_include_directories(divsufsort64 PUBLIC "${divsufsort_BINARY_DIR}/include")
endif()

if(divsufsort64)
    message("divsufsort64 set")
endif(divsufsort64)



include_directories(external/cst.jouni/libraries/TextIndexes/includes)
include_directories(external/cst.jouni/libraries/libcds2/includes)
include_directories(external/cst.jouni/libraries/tools/includes)



add_subdirectory(src)
#add_subdirectory(tests)
add_subdirectory(experiments)

add_subdirectory(external/cst.jouni)
add_subdirectory(external/repair_gn)


