cmake_minimum_required(VERSION 3.15)

project(Physics_Engine LANGUAGES C CXX Fortran CUDA)

# Language standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_Fortran_STANDARD 2003)
set(CMAKE_Fortran_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# "native" queries the physical GPU present on the machine, which CI runners
# don't have. Only default to it when nothing was passed on the command line
# (e.g. `-DCMAKE_CUDA_ARCHITECTURES=all-major` for a GPU-less build).
if(NOT CMAKE_CUDA_ARCHITECTURES)
    set(CMAKE_CUDA_ARCHITECTURES "native")
endif()

find_package(OpenMP REQUIRED)

enable_testing()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Define subdirectories (modular libraries and main build target)
set(SUBDIRECTORIES
    src                 # Main application and entry point
    src/math            # Math utilities (linear algebra, calculus)
    src/models          # Data models (physics objects, bodies, etc.)
    src/logic           # Physics logic (gravity, forces, etc.)
    test                # Unit tests
)

# Include each subdirectory
foreach(SUBDIR ${SUBDIRECTORIES})
    add_subdirectory(${SUBDIR})
endforeach()
