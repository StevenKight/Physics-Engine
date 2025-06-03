cmake_minimum_required(VERSION 3.15)

project(Physics_Engine LANGUAGES C CXX Fortran CUDA)

# Language standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_Fortran_STANDARD 2003)
set(CMAKE_Fortran_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_ARCHITECTURES "native")

# Define subdirectories (modular libraries and main build target)
set(SUBDIRECTORIES
    src            # Main application and entry point
    src/fortran      # Fortran sublibrary (tight loop utilities)
    src/matrix     # CUDA sublibrary (matrix ops, GPU accel)
    src/io         # I/O parsing and serialization
    src/state      # Data models for simulation objects
)

# Include each subdirectory
foreach(SUBDIR ${SUBDIRECTORIES})
    add_subdirectory(${SUBDIR})
endforeach()
