# Add the CUDA and Fortran subdirectories
add_subdirectory(cuda)
add_subdirectory(fortran)

# Create the main math library that wraps both submodules
add_library(math_lib
    matrix.c
)

# Expose this directory for includes (so you can #include "matrix.h")
target_include_directories(math_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link to CUDA and Fortran math libraries
target_link_libraries(math_lib INTERFACE
    cuda_lib
    fortran_lib
)

# If you use CUDA defines or compiler flags for all math code
target_compile_definitions(math_lib PRIVATE -DUSE_CUDA)
