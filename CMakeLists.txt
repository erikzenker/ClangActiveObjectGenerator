cmake_minimum_required(VERSION 3.2.2)
project(clang_active_object)
set (CMAKE_CXX_STANDARD 14)

# Add mstch dependency
add_subdirectory(external/mstch)
include_directories(${mstch_INCLUDE_DIR})

# Add clang plugin
add_llvm_loadable_module(clang_active_object ClangActiveObject.cpp PLUGIN_TOOL clang)
target_compile_options(clang_active_object PRIVATE -fexceptions)
target_link_libraries(clang_active_object mstch)

if(LLVM_ENABLE_PLUGINS AND (WIN32 OR CYGWIN))
  target_link_libraries(clang_active_object PRIVATE
    clangAST
    clangBasic
    clangFrontend
    LLVMSupport
    )
endif()

# Example
add_subdirectory(example)