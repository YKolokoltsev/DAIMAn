cmake_minimum_required (VERSION 3.1)

project (DAIMAn)

set_property(GLOBAL PROPERTY CXX_STANDARD 11)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED true)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/modules/")

add_executable(test_concept test_concept.cpp)
set_property(TARGET test_concept PROPERTY CXX_STANDARD 11)

add_subdirectory(src)