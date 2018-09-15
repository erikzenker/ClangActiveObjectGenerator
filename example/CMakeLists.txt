cmake_minimum_required(VERSION 3.2.2)
project(clang_active_object_example)

# Add boost dependency
find_package(
        Boost 1.54.0
        COMPONENTS system filesystem unit_test_framework
        REQUIRED
)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# Add thread dependecy
find_package(Threads)

# Add example
add_executable(clang_active_object_example main.cpp)
target_link_libraries(clang_active_object_example PRIVATE
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)