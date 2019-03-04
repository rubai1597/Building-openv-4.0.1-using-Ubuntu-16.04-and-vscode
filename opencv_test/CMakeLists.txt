project(opencv_test)

set (CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.0)
find_package(OpenCV REQUIRED)
 
file(GLOB SOURCES  *.cpp)

SET(OpenCV_DIR /usr/local/lib/cmake/opencv4) 

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries( opencv_test ${OpenCV_LIBS} )