cmake_minimum_required(VERSION 3.2.2)
project(clang_active_object_generator)
set (CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
set(LIB_NAME clang_active_object_generator)

# Add clang plugin target
add_library(${LIB_NAME} SHARED src/ClangActiveObject.cpp)

# Add mstch dependency
add_subdirectory(external/mstch)

# Add libclang dependecy
find_package(
        LibClang
        MODULE
        REQUIRED
)

target_include_directories(${LIB_NAME} PUBLIC
        ${mstch_INCLUDE_DIR}
        ${LibClang_INCLUDE_DIRS}
)

target_compile_options(${LIB_NAME} PRIVATE -fexceptions)
target_link_libraries(${LIB_NAME} PRIVATE
    mstch
    ${LIBCLANG_LIBRARIES}
)

# Add install target
install(
    TARGETS ${LIB_NAME}
    LIBRARY DESTINATION lib
)

install(
    FILES src/clang-active-object-generator
    PERMISSIONS WORLD_EXECUTE WORLD_WRITE WORLD_READ
    DESTINATION bin
)

install(
    FILES src/ActiveObject.mustache
    DESTINATION share/clang-active-object-generator
)

# Add examples
# add_subdirectory(example)

# Add tests
# add_subdirectory(test)