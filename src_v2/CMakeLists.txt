find_package(Boost REQUIRED)
find_package(OpenBabel2 REQUIRED)
find_package (Threads)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(CMAKE_FIND_ROOT_PATH "/home/morrigan/Opt/OpenQt5.9/5.9.1/gcc_64/")
set(CMAKE_FIND_ROOT_PATH "/home/morrigan/Opt/Qt5/5.11.0/gcc_64")
find_package(Qt5 REQUIRED COMPONENTS
        Core Gui Widgets Xml OpenGL 3DCore 3DInput 3DRender 3DExtras)

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Xml_DEFINITIONS})
add_definitions(${Qt5OpenGL_DEFINITIONS})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/widgets")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/document")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/data")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/multithread")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/multithread/tasks")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/widgets/custom_3d")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/widgets/tree_items")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/widgets/tree_items/critical_points")

include_directories("${OPENBABEL2_INCLUDE_DIR}")
message(STATUS "OpenBabel2: ${OPENBABEL2_INCLUDE_DIR}")

#specials
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

file(GLOB DAIMAn2_SRC
        "*.cpp"
        "widgets/*.cpp"
        "document/*.cpp"
        "document/*.hpp"
        "data/*.cpp"
        "multithread/*.cpp"
        "multithread/*.hpp"
        "multithread/tasks/*.cpp"
        "multithread/tasks/*.hpp"
        "widgets/custom_3d/*.cpp"
        "widgets/custom_3d/*.hpp"
        "widgets/tree_items/*.cpp"
        "widgets/tree_items/*.hpp"
        "widgets/tree_items/critical_points/*.cpp"
        "widgets/tree_items/critical_points/*.hpp")

add_executable(DAIMAn2 ${DAIMAn2_SRC})
set_property(TARGET DAIMAn2 PROPERTY CXX_STANDARD 11)

target_link_libraries(DAIMAn2 ${Boost_LIBRARIES})
target_link_libraries (DAIMAn2 ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(DAIMAn2 ${OPENBABEL2_LIBRARIES})
target_link_libraries(DAIMAn2
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Xml
        Qt5::3DCore
        Qt5::3DInput
        Qt5::3DRender
        Qt5::3DExtras)

