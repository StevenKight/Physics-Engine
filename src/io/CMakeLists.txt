# I/O library for reading simulation files
add_library(io_lib
    parser.cpp
)

# Expose headers in this directory to consumers
target_include_directories(io_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link the core simulation state library
target_link_libraries(io_lib PUBLIC state_lib)
