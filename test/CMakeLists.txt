cmake_minimum_required(VERSION 3.15)

project(physics_tests C)

set(TEST_SOURCES
    math/test_matrix_add.c
    math/test_matrix_sub.c
    math/test_matrix_mul.c
    math/test_matrix_scalar.c
    math/test_matrix_power.c
)

foreach(src IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${src} NAME_WE)

    add_executable(${test_name} ${src})

    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/math
        ${CMAKE_CURRENT_SOURCE_DIR}/framework
    )

    target_link_libraries(${test_name} PRIVATE math_lib)

    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
