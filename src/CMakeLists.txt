
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(CGAL REQUIRED COMPONENTS Core)
find_package(LAPACKE REQUIRED)
find_package(LAPACK REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Xml_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5OpenGL_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Xml_DEFINITIONS})
add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5OpenGL_DEFINITIONS})

#specials
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
include( ${CGAL_USE_FILE} )

#include headers from subdirs
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/camera")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/gui")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/old_misc")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/wavefunction")

#executable
file(GLOB DAIMAn_SRC
        "*.cpp"
        "wavefunction/*.cpp"
        "old_misc/*.cpp"
        "camera/*.cpp"
        "gui/*.cpp")

add_executable(DAIMAn ${DAIMAn_SRC})
set_property(TARGET DAIMAn PROPERTY CXX_STANDARD 11)

target_link_libraries(DAIMAn ${Qt5Widgets_LIBRARIES})
target_link_libraries(DAIMAn ${Qt5Xml_LIBRARIES})
target_link_libraries(DAIMAn ${Qt5Core_LIBRARIES})
target_link_libraries(DAIMAn ${Qt5OpenGL_LIBRARIES})
target_link_libraries(DAIMAn ${OPENGL_LIBRARIES})
target_link_libraries(DAIMAn ${LAPACKE_LIBRARIES})
target_link_libraries(DAIMAn ${LAPACK_LIBRARIES})

#message(STATUS "USR_MSG: ${CMAKE_CURRENT_SOURCE_DIR}/gui")