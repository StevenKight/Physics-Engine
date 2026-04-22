cmake_minimum_required(VERSION 3.15)

project(physics_tests C)

set(MATH_TEST_SOURCES
    math/test_matrix_add.c
    math/test_matrix_sub.c
    math/test_matrix_mul.c
    math/test_matrix_scalar.c
    math/test_matrix_power.c
)

foreach(src IN LISTS MATH_TEST_SOURCES)
    get_filename_component(test_name ${src} NAME_WE)

    add_executable(${test_name} ${src})

    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/math
        ${CMAKE_CURRENT_SOURCE_DIR}/framework
    )

    target_link_libraries(${test_name} PRIVATE math_lib)

    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

set(MODELS_TEST_SOURCES
    models/test_object_step.c
)

foreach(src IN LISTS MODELS_TEST_SOURCES)
    get_filename_component(test_name ${src} NAME_WE)

    add_executable(${test_name} ${src})

    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/models
        ${CMAKE_SOURCE_DIR}/src/math
        ${CMAKE_CURRENT_SOURCE_DIR}/framework
    )

    target_link_libraries(${test_name} PRIVATE models_lib math_lib)

    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

set(LOGIC_TEST_SOURCES
    logic/test_newtonian_gravity.c
    logic/test_aabb.c
    logic/test_collision.c
    logic/test_inelastic_collision.c
)

foreach(src IN LISTS LOGIC_TEST_SOURCES)
    get_filename_component(test_name ${src} NAME_WE)

    add_executable(${test_name} ${src})

    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/logic
        ${CMAKE_SOURCE_DIR}/src/logic/forces
        ${CMAKE_SOURCE_DIR}/src/logic/collision
        ${CMAKE_SOURCE_DIR}/src/models
        ${CMAKE_SOURCE_DIR}/src/math
        ${CMAKE_CURRENT_SOURCE_DIR}/framework
    )

    target_link_libraries(${test_name} PRIVATE logic_lib forces_lib collision_lib)

    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
