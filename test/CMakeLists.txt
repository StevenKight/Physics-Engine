cmake_minimum_required(VERSION 3.15)

project(physics_tests C)

# List of test source files (one test executable per file)
set(TEST_SOURCES
    test_matrix_add.c
    test_matrix_sub.c
    test_matrix_mul.c
    test_matrix_scalar_mul.c
    test_matrix_scalar_div.c
    test_matrix_scalar_add.c
    test_matrix_scalar_sub.c
)

foreach(src IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${src} NAME_WE)

    add_executable(${test_name} ${src})

    # Include project headers
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/math
        ${CMAKE_SOURCE_DIR}/test
    )

    # Link against math_lib (the wrapper library)
    target_link_libraries(${test_name} PRIVATE math_lib)

    # Register the test with CTest (name matches executable)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
