cmake_minimum_required(VERSION 3.0)
project(ChameLib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_BUILD_TYPE "Release")

include_directories(include)

find_package(OpenSSL REQUIRED)

file(GLOB LIBRARY_SOURCES
    src/AE/*.cpp
    src/base/*.cpp
    src/exception/*.cpp
    src/curve/*.cpp
    src/utils/*.cpp
    src/SE/*.cpp
    src/signature/*.cpp
    src/ABE/*.cpp
    src/scheme/CH/*.cpp
    src/scheme/IBCH/*.cpp
    src/scheme/PBCH/*.cpp
)
add_library(ChameLib STATIC ${LIBRARY_SOURCES})
target_link_libraries(ChameLib PUBLIC gmp pbc OpenSSL::SSL)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -mavx2 -O0")
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
    find_package(GTest REQUIRED)

    enable_testing()

    macro(add_basetest test_name test_source)
        add_executable(${test_name} ${test_source})
        target_include_directories(${test_name} PRIVATE)
        target_link_libraries(${test_name} PRIVATE ChameLib GTest::GTest pthread)

        add_test(NAME "${test_name}" COMMAND ${test_name})
        set_tests_properties("${test_name}" PROPERTIES LABELS "basetest")
    endmacro()

    file(GLOB BASETEST_SOURCES "test/scheme_test/CH_test/*.cpp" "test/scheme_test/IBCH_test/*.cpp" "test/scheme_test/PBCH_test/*.cpp")
    foreach(test_source ${BASETEST_SOURCES})
        get_filename_component(test_name ${test_source} NAME_WE)
        add_basetest(${test_name} ${test_source})
    endforeach()

    macro(add_benchmarktest test_name test_source)
        add_executable(${test_name} ${test_source})
        target_include_directories(${test_name} PRIVATE test/common)
        target_link_libraries(${test_name} PRIVATE ChameLib GTest::GTest pthread)

        add_test(NAME "${test_name}" COMMAND ${test_name})
        set_tests_properties("${test_name}" PROPERTIES LABELS "benchmarktest")
    endmacro()

    file(GLOB BENCHMARKTEST_SOURCES "test/scheme_test/benchmark/*/*.cpp" "test/scheme_test/benchmark/*.cpp")
    foreach(test_source ${BENCHMARKTEST_SOURCES})
        get_filename_component(test_name ${test_source} NAME_WE)
        add_benchmarktest(${test_name} ${test_source})
    endforeach()
endif()

install(TARGETS ChameLib ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include/ChameLib)
