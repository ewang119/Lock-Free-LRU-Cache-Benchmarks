cmake_minimum_required(VERSION 3.13)
project(FLOCK VERSION 1.0 DESCRIPTION "Lock-Free Locks" LANGUAGES CXX)


set(FLOCK_INCLUDE_DIR "/users/KaiserW/diagnosis/include")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -lbenchmark -pthread")
message(STATUS "Where tf is flock?:           ${FLOCK_INCLUDE_DIR}" )

add_library(flock INTERFACE)
target_include_directories(flock INTERFACE ${FLOCK_INCLUDE_DIR})
target_compile_features(flock INTERFACE cxx_std_17)
target_compile_options(flock INTERFACE -mcx16 -march=native -g)
find_package(Threads REQUIRED)

message(STATUS "CMAKE_CXX_COMPILER:        ${CMAKE_CXX_COMPILER}")
  message(STATUS "Downloading a local copy of ParlayLib")
  # Download and configure GoogleTest
  include(FetchContent)
  FetchContent_Declare(parlaylib
    GIT_REPOSITORY  https://github.com/cmuparlay/parlaylib.git
    GIT_TAG         master
  )
  FetchContent_GetProperties(parlaylib)
  if(NOT parlaylib_POPULATED)
    FetchContent_Populate(parlaylib)  
    add_subdirectory(${parlaylib_SOURCE_DIR} EXCLUDE_FROM_ALL)
  endif()
  target_link_libraries(flock INTERFACE parlay)

find_library(JEMALLOC_LIB jemalloc)
if(NOT JEMALLOC_LIB)
  message(FATAL_ERROR "Could not find jemalloc.")
else()
  message("-- Found jemalloc: ${JEMALLOC_LIB}")
  target_link_libraries(flock INTERFACE optimized ${JEMALLOC_LIB})
endif()
target_link_libraries(flock INTERFACE benchmark)
target_link_libraries(flock INTERFACE pthread)

set(SOURCES flock_lru.cpp)
add_executable(FlockLRU flock_lru.cpp)
# add_subdirectory(include)
target_link_libraries(FlockLRU flock)