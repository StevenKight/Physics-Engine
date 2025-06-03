# Locate the CUDA Toolkit
find_package(CUDAToolkit REQUIRED)

# Create a library from CUDA sources
add_library(matrix_lib
    matrix.cu
)

# Enable separable compilation for CUDA kernels
set_target_properties(matrix_lib PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
)

# Expose this directory as an include path to dependents
target_include_directories(matrix_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against the CUDA runtime (required for cudaMalloc, cudaMemcpy, etc.)
target_link_libraries(matrix_lib PRIVATE CUDA::cudart)
