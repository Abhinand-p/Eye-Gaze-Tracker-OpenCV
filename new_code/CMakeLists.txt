cmake_minimum_required(VERSION 3.28)

project(Eye_Gaze_Tracker_OpenCV)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Eye_Gaze_Tracker_OpenCV main.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})