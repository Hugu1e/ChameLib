cmake_minimum_required(VERSION 3.0)
project(ChameLib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_BUILD_TYPE "Release")

include_directories(include libs/include/gmp libs/include/pbc libs/include/openssl tests/common)

link_directories(libs/lib)

find_package(OpenSSL REQUIRED)
message(STATUS "OpenSSL include directory: ${OpenSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL SSL library: ${OpenSSL_SSL_LIBRARY}")
message(STATUS "OpenSSL Crypto library: ${OpenSSL_CRYPTO_LIBRARY}")


file(GLOB LIBRARY_SOURCES
    src/AE/*.cpp
    src/base/*.cpp
    src/curve/*.cpp
    src/utils/*.cpp
    src/SE/*.cpp
    src/signature/*.cpp
    src/ABE/*.cpp
)
add_library(ChameLib STATIC ${LIBRARY_SOURCES})
target_link_libraries(ChameLib PUBLIC gmp pbc OpenSSL::SSL)


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

SET(BUILD_TESTS ON)
if(BUILD_TESTS)
    enable_testing()

    macro(add_test_executable test_name test_source)
        add_executable(${test_name} ${test_source} tests/common/CommonTest.cpp)
        target_link_libraries(${test_name} PRIVATE ChameLib)
        add_test(NAME ${test_name} COMMAND ${test_name})
    endmacro()

    add_test_executable(RSA_test tests/RSA_test/RSA_test.cpp)
    add_test_executable(Elements_test tests/base_test/Elements_test.cpp)
    add_test_executable(AES_test tests/SE_test/AES_test.cpp)
    add_test_executable(BLS_test tests/signature_test/BLS_test.cpp)
    add_test_executable(CP_ABE_test tests/ABE_test/CP_ABE_test.cpp)
    add_test_executable(MA_ABE_test tests/ABE_test/MA_ABE_test.cpp)
    add_test_executable(ABET_test tests/ABE_test/ABET_test.cpp)
endif()

install(TARGETS ChameLib ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
