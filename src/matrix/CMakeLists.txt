# Find the CUDA package
find_package(CUDAToolkit REQUIRED)

# Add your CUDA sources
set(CUDA_SOURCES
    matrix.cu matrix.h
)

# Create a library from the CUDA sources
add_library(matrix_library ${CUDA_SOURCES})

# Include directories for the library
target_include_directories(matrix_library PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}  # Adds the current directory (which should contain utils.h) to the include path
)

# Specify the properties for CUDA compilation
set_target_properties(matrix_library PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
)

# Link libraries if needed
target_link_libraries(matrix_library PRIVATE CUDA::cudart)
