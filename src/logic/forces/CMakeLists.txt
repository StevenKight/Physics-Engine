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
