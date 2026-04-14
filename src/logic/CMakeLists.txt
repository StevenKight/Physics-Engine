add_subdirectory(forces)

file(GLOB LOGIC_SOURCES "*.c")

add_library(logic_lib
    ${LOGIC_SOURCES}
)

# Expose this directory for includes (so you can #include "sim.h", etc.)
target_include_directories(logic_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(logic_lib PUBLIC
    math_lib
    models_lib
    forces_lib
)

# Shared library for Python ctypes bindings.
# Links against the same static libs as logic_lib, preserving the full
# CUDA/Fortran dispatch path in gravity.
#
# Pre-loading libcudart.so with RTLD_GLOBAL from Python before importing
# this library is required so the CUDA device-code registration constructors
# that run at dlopen time find an initialised runtime. See interface/nbody.py.
add_library(nbody_sim SHARED
    ${LOGIC_SOURCES}
)

target_include_directories(nbody_sim PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(CUDAToolkit REQUIRED)

target_link_libraries(nbody_sim PUBLIC
    math_lib
    models_lib
    forces_lib
)

# Explicitly add Fortran and CUDA runtime dynamic deps.
# CMake picks the C linker for a C-only SHARED target and won't inject these
# automatically the way it does for executables.
target_link_libraries(nbody_sim PRIVATE
    ${CMAKE_Fortran_IMPLICIT_LINK_LIBRARIES}
    CUDA::cudart
)
