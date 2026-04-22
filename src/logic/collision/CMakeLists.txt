file(GLOB COLLISION_SOURCES "*.c")

add_library(collision_lib
    ${COLLISION_SOURCES}
)

target_include_directories(collision_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(collision_lib PUBLIC
    math_lib
    models_lib
    OpenMP::OpenMP_C
)
