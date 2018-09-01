cmake_minimum_required(VERSION 3.2.2)
project(active_object)

set (CMAKE_CXX_STANDARD 14)

find_package(
        Boost 1.54.0
        COMPONENTS system filesystem unit_test_framework
        REQUIRED
)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_package(Threads)

add_executable(active_object main.cpp)

target_link_libraries(
  active_object
  PUBLIC ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
)