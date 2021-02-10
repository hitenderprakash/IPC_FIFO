
cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 11)
project(wrt)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(${CMAKE_CURRENT_LIST_DIR})
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(${PROJECT_NAME}_SOURCE_FILES 
    "${CMAKE_CURRENT_LIST_DIR}/wrt.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/fd.cpp"

)
add_executable(${PROJECT_NAME}  ${${PROJECT_NAME}_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

project(rdr)
include_directories(${CMAKE_CURRENT_LIST_DIR})
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(${PROJECT_NAME}_SOURCE_FILES 
    "${CMAKE_CURRENT_LIST_DIR}/rdr.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/fd.cpp"

)
add_executable(${PROJECT_NAME}  ${${PROJECT_NAME}_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)