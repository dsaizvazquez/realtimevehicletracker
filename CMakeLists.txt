cmake_minimum_required (VERSION 3.2)

project ("opencv_test")
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -latomic")

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package( PkgConfig REQUIRED )
include_directories(/usr/include/opencv2)
include_directories(${CMAKE_SOURCE_DIR}/libs)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(external/yaml-cpp)
add_subdirectory(external/jsoncpp)

