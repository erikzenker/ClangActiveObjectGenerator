cmake_minimum_required(VERSION 3.2.2)
project(clang_active_object_generator_example)

# Generate the active objects
execute_process (
    COMMAND bash -c "../clang-active-object-generator \
                        ${CMAKE_CURRENT_SOURCE_DIR}/ICalculator.hpp ICalculator \
                      > ${CMAKE_CURRENT_SOURCE_DIR}/ICalculatorActiveObject.hpp"
    OUTPUT_VARIABLE ICalculatorGeneratorOutput
)
execute_process (
    COMMAND bash -c "../clang-active-object-generator \
                         ${CMAKE_CURRENT_SOURCE_DIR}/IResultObserver.hpp IResultObserver \
                       > ${CMAKE_CURRENT_SOURCE_DIR}/IResultObserverActiveObject.hpp"
    OUTPUT_VARIABLE IResultObserverGeneratorOutput
)

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
add_executable(clang_active_object_generator_example main.cpp)
target_link_libraries(clang_active_object_generator_example PRIVATE
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)