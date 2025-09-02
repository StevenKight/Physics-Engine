# Create a header-only interface library for simulation state
add_library(state_lib INTERFACE)

# Expose this directory as a public include path
target_include_directories(state_lib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link to matrix_lib to access matrix.h
target_link_libraries(state_lib INTERFACE matrix_lib)
