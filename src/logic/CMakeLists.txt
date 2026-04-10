# Collect all .c source files in this directory
file(GLOB LOGIC_SOURCES "*.c")

# Create the logic library
add_library(logic_lib
    ${LOGIC_SOURCES}
)

# Expose this directory for includes (so you can #include "gravity.h", etc.)
target_include_directories(logic_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link to math_lib to access matrix.h and related functions
target_link_libraries(logic_lib PUBLIC
    math_lib
    models_lib
)
