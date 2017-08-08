cmake_minimum_required (VERSION 3.1)

project (DAIMAn)

set_property(GLOBAL PROPERTY CXX_STANDARD 11)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED true)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/modules/")

find_package(Boost REQUIRED) #BGL routines
find_package (Threads REQUIRED)
include_directories("${Boost_INCLUDE_DIR}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src_v2/document")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src_v2/data")

file(GLOB TEST_CONCEPT_SRC
        "test_concept.cpp"
        "src_v2/document/*.cpp"
        "src_v2/document/*.hpp"
        "src_v2/data/*.hpp")

add_executable(test_concept ${TEST_CONCEPT_SRC})
set_property(TARGET test_concept PROPERTY CXX_STANDARD 11)

target_link_libraries(test_concept ${Boost_LIBRARIES})
target_link_libraries (test_concept ${CMAKE_THREAD_LIBS_INIT})

#add_subdirectory(src)
add_subdirectory(src_v2)