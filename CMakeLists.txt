cmake_minimum_required(VERSION 3.2.2)
project(clang_active_object)
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
add_library(clang_active_object SHARED ClangActiveObject.cpp)

target_compile_options(clang_active_object PRIVATE -fexceptions)
#target_compile_options(clang_active_object PRIVATE -fPIC)
target_link_libraries(clang_active_object PRIVATE
        mstch
        ${LIBCLANG_LIBRARIES}
        )

# Example
add_subdirectory(example)