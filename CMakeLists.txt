cmake_minimum_required(VERSION 3.10)

project(CipheyCore VERSION 0.3.0.3)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Boost REQUIRED COMPONENTS)

file(GLOB_RECURSE ciphey_core_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(ciphey_core ${ciphey_core_SOURCES})
target_include_directories(ciphey_core PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
include_directories(subboost)

if (MSVC)
  target_compile_options(ciphey_core PUBLIC "/std:c++latest")
else()
  target_compile_features(ciphey_core PUBLIC cxx_std_20)
endif()

target_link_libraries(ciphey_core PUBLIC Boost::boost)

add_library(Ciphey::core ALIAS ciphey_core)

set(UseSWIG_TARGET_NAME_PREFERENCE STANDARD)

option(CIPHEY_CORE_PYTHON "Force including (and linking if necessary) from the python installation in the given directory")
if (CIPHEY_CORE_PYTHON)
  if (APPLE)
    file(GLOB python_include "${CIPHEY_CORE_PYTHON}/include/python*")
    file(GLOB python_lib "${CIPHEY_CORE_PYTHON}/lib/config*/libpython*.a")
    include_directories(${python_include})
    set(ciphey_py_extra ${python_lib})
  elseif (EXISTS "${CIPHEY_CORE_PYTHON}/include/")
    include_directories("${CIPHEY_CORE_PYTHON}/include")
    link_directories("${CIPHEY_CORE_PYTHON}/libs")
  else()
    include_directories(${CIPHEY_CORE_PYTHON})
  endif()
else()
  set(Python3_USE_STATIC_LIBS)
  find_package(Python3 REQUIRED COMPONENTS Development)
  include_directories(${Python3_INCLUDE_DIRS})
  set(ciphey_py_extra Python3::Python)
endif()

find_package(SWIG REQUIRED)

include(${SWIG_USE_FILE})
set(PYTHON_SWIG ${CMAKE_CURRENT_SOURCE_DIR}/py/python.i)
set_property(SOURCE ${PYTHON_SWIG} PROPERTY CPLUSPLUS ON)
set_property(SOURCE ${PYTHON_SWIG} PROPERTY USE_TARGET_INCLUDE_DIRECTORIES TRUE)
swig_add_library(ciphey_core_py
  TYPE SHARED
  LANGUAGE python
  OUTFILE_DIR "${CMAKE_CURRENT_BINARY_DIR}"
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/cipheycore"
  SOURCES ${PYTHON_SWIG})

# Apple needs ".so", rather than its default ".dylib"
if (APPLE)
  set_target_properties(ciphey_core_py PROPERTIES SUFFIX ".so")
endif()

target_link_libraries(ciphey_core_py Ciphey::core ${ciphey_py_extra})
add_custom_command(TARGET ciphey_core_py PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/cipheycore"
)

if (PROJECT_VERSION_TWEAK)
  set(CIPHEY_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-rc${PROJECT_VERSION_TWEAK}")
else()
  set(CIPHEY_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/py/pyproject.toml.in ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml)

add_custom_command(TARGET ciphey_core_py POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_CURRENT_BINARY_DIR}/cipheycore/cipheycore.py" "${CMAKE_CURRENT_BINARY_DIR}/cipheycore/__init__.py"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/py/build.py" "${CMAKE_CURRENT_BINARY_DIR}/"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:ciphey_core_py>"  "${CMAKE_CURRENT_BINARY_DIR}/cipheycore/"
)

option(CIPHEY_CORE_TEST "Enable testing" ON)

if (CIPHEY_CORE_TEST)
  message("Testing enabled")
  enable_testing()
  find_package(GTest REQUIRED)
  include(GoogleTest)
  file(GLOB_RECURSE ciphey_core_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cxx)
  add_executable(ciphey_core_test ${ciphey_core_TESTS})
  target_link_libraries(ciphey_core_test Ciphey::core GTest::GTest GTest::Main)
  gtest_discover_tests(ciphey_core_test)
endif()
