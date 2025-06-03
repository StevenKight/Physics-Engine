# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# Set CUDA specific flags if necessary
set(CMAKE_CUDA_ARCHITECTURES "native")

# Set the project name and languages
project(Physics_Engine LANGUAGES C CXX Fortran CUDA)

# Set the C++ standard, Fortran standard, or other language-specific flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_Fortran_STANDARD 2003)

# Add subdirectories where code is located
set(SUBDIRECTORIES
    src src/array src/matrix src/io src/state
)

# Include the subdirectories
foreach(subdir ${SUBDIRECTORIES})
    add_subdirectory(${subdir})
endforeach()
