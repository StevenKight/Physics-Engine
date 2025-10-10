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
    src                 # Main application and entry point
    src/math            # Math utilities (linear algebra, calculus)
)

# Include each subdirectory
foreach(SUBDIR ${SUBDIRECTORIES})
    add_subdirectory(${SUBDIR})
endforeach()
