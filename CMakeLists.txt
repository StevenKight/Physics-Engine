# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# Set CUDA specific flags if necessary
set(CMAKE_CUDA_ARCHITECTURES "native")

# Set the project name and languages
project(Physics_Engine LANGUAGES C CXX Fortran CUDA)

# Set the C++ standard, Fortran standard, or other language-specific flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_Fortran_STANDARD 2003)

# Add subdirectories where your code is located
add_subdirectory(src)
add_subdirectory(src/fortran)
add_subdirectory(src/matrix)