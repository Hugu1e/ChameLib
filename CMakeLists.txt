cmake_minimum_required(VERSION 3.0)
project(ChameLib)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_BUILD_TYPE "Release")

include_directories(${CMAKE_SOURCE_DIR}/libs/gmp/include)
include_directories(libs/pbc/include)
include_directories(include)


set(GMP_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/libs/gmp/lib/libgmp.so)
if (EXISTS ${GMP_LIBRARY_PATH})
    message(STATUS "GMP library found at: ${GMP_LIBRARY_PATH}")
else()
    message(FATAL_ERROR "GMP library not found at: ${GMP_LIBRARY_PATH}")
endif()
# link_libraries(${GMP_LIBRARY_PATH})





set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -mavx2 -O0")
add_compile_options(-Wunknown-pragmas)

FIND_PACKAGE(OpenMP REQUIRED)
if (OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif ()

if (ENABLE_OMP)
    message("OpenMP enabled")
    add_definitions("-DParallel")
endif (ENABLE_OMP)

if (LOCAL_DEBUG)
    message("LOCAL_DEBUG enabled")
    add_definitions("-DDEBUG")
endif (LOCAL_DEBUG)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}")

add_executable(RSA_test
        tests/RSA_test/RSA_test.cpp
        src/AE/RSA.cpp
        src/utils/RandomGenerator.cpp
        )
target_link_libraries(RSA_test ${GMP_LIB_PATH})

