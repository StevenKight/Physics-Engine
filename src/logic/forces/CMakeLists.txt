file(GLOB FORCES_SOURCES "*.c")

add_library(forces_lib
    ${FORCES_SOURCES}
)

target_include_directories(forces_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(forces_lib PUBLIC
    math_lib
    models_lib
)

# gravity.c only routes large N to the CUDA backend when USE_CUDA is defined.
# The define on math_lib is PRIVATE, so it has to be set here as well.
target_compile_definitions(forces_lib PRIVATE -DUSE_CUDA)
