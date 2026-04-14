add_subdirectory(forces)

file(GLOB LOGIC_SOURCES "*.c")

add_library(logic_lib
    ${LOGIC_SOURCES}
)

# Expose this directory for includes (so you can #include "sim.h", etc.)
target_include_directories(logic_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(logic_lib PUBLIC
    math_lib
    models_lib
    forces_lib
)
