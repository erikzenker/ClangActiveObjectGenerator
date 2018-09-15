cmake_minimum_required(VERSION 3.2.2)

# Generate the active objects
execute_process (
        COMMAND bash -c "cd ${CMAKE_CURRENT_SOURCE_DIR} && ./generate_and_compare_to_reference.sh"
        RESULT_VARIABLE TestResult
)

if (${TestResult} STREQUAL "1")
    message(FATAL_ERROR "Generated code is not equal to reference")
endif ()

add_custom_target(clang_active_object_generator_unit_tests
        SOURCES
          ${PROJECT_SOURCE_DIR}/ActiveObject.mustache)