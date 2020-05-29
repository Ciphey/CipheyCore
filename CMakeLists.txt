cmake_minimum_required(VERSION 3.10)

project(CipheyCore)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Boost REQUIRED COMPONENTS)

file(GLOB_RECURSE ciphey_core_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(ciphey_core ${ciphey_core_SOURCES})
target_include_directories(ciphey_core PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(ciphey_core PUBLIC cxx_std_17)

target_link_libraries(ciphey_core PUBLIC Boost::boost)

add_library(Ciphey::core ALIAS ciphey_core)

find_package(Python3 REQUIRED COMPONENTS Development)
find_package(SWIG REQUIRED)

include(${SWIG_USE_FILE})
set(PYTHON_SWIG ${CMAKE_CURRENT_SOURCE_DIR}/swig/python.i)
set_property(SOURCE ${PYTHON_SWIG} PROPERTY CPLUSPLUS ON)
set_property(SOURCE ${PYTHON_SWIG} PROPERTY USE_TARGET_INCLUDE_DIRECTORIES TRUE)
swig_add_library(ciphey_core_py
  TYPE SHARED
  LANGUAGE python
  SOURCES ${PYTHON_SWIG})
swig_link_libraries(ciphey_core_py Ciphey::core Python3::Python)

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)
file(GLOB_RECURSE ciphey_core_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cxx)
add_executable(ciphey_core_test ${ciphey_core_TESTS})
target_link_libraries(ciphey_core_test Ciphey::core GTest::GTest GTest::Main)
gtest_discover_tests(ciphey_core_test)
