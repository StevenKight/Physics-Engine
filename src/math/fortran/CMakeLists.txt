# Ensure Fortran support is enabled (can be skipped if already done at the root)
enable_language(Fortran)

# Create a Fortran + C interface library
add_library(fortran_lib
    matrix_add.f90
    matrix_sub.f90
    matrix_scalar.f90
    matrix_mul.f90
)

# Expose the directory so the C++ code can include the Fortran headers
target_include_directories(fortran_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)