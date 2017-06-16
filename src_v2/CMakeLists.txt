find_package(Boost REQUIRED) #BGL routines

include_directories("${Boost_INCLUDE_DIR}")

file(GLOB DAIMAn2_SRC "*.cpp")
add_executable(DAIMAn2 ${DAIMAn2_SRC})
set_property(TARGET DAIMAn2 PROPERTY CXX_STANDARD 11)
target_link_libraries(DAIMAn2 ${Boost_LIBRARIES})

