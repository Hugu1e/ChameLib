cmake_minimum_required(VERSION 3.0)
project(ChameLib)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_BUILD_TYPE "Release")

include_directories(include libs/include/gmp libs/include/pbc tests/common)

link_directories(libs/lib)

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
        src/utils/Logger.cpp
        src/utils/RandomGenerator.cpp
)
target_link_libraries(RSA_test gmp)


add_executable(Elements_test
        tests/base_test/Elements_test.cpp
        src/base/PbcElements.cpp
        src/curve/params.cpp
        tests/common/CommonTest.cpp
)
target_link_libraries(Elements_test pbc)

