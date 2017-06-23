find_package(Boost REQUIRED) #BGL routines
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(OpenGL REQUIRED)

include_directories("${Boost_INCLUDE_DIR}")
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Xml_INCLUDE_DIRS})
include_directories(${Qt5OpenGL_INCLUDE_DIRS})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/widgets")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/document")

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Xml_DEFINITIONS})
add_definitions(${Qt5OpenGL_DEFINITIONS})

#specials
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

file(GLOB DAIMAn2_SRC
        "*.cpp"
        "widgets/*.cpp"
        "document/*.cpp"
        "document/*.hpp")

add_executable(DAIMAn2 ${DAIMAn2_SRC})
set_property(TARGET DAIMAn2 PROPERTY CXX_STANDARD 11)

target_link_libraries(DAIMAn2 ${Boost_LIBRARIES})
target_link_libraries(DAIMAn2 ${Qt5Widgets_LIBRARIES})
target_link_libraries(DAIMAn2 ${Qt5Xml_LIBRARIES})
target_link_libraries(DAIMAn2 ${Qt5OpenGL_LIBRARIES})
target_link_libraries(DAIMAn2 ${OPENGL_LIBRARIES})

