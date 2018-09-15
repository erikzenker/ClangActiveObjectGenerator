cmake_minimum_required(VERSION 3.2.2)
project(clang_active_object_generator)
set (CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

# Add mstch dependency
add_subdirectory(external/mstch)
include_directories(${mstch_INCLUDE_DIR})

# Add libclang dependecy
find_package(
        LibClang
        MODULE
        REQUIRED
)
include_directories(SYSTEM ${LibClang_INCLUDE_DIRS})

# Add clang plugin
add_library(clang_active_object_generator SHARED ClangActiveObject.cpp)

target_compile_options(clang_active_object_generator PRIVATE -fexceptions)
target_link_libraries(clang_active_object_generator PRIVATE
        mstch
        ${LIBCLANG_LIBRARIES}
        )

# Example
add_subdirectory(example)