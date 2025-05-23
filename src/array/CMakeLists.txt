# Add your Fortran source files
set(FORTRAN_SOURCES
    array.h array.f90
)

# Create a library from Fortran sources
add_library(fortran_library ${FORTRAN_SOURCES})

# Ensure Fortran compiler is used
enable_language(Fortran)

# Include directories for the library
target_include_directories(fortran_library PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}  # Adds the current directory (which should contain utils.h) to the include path
)

# Link the Fortran library with the main executable
target_link_libraries(fortran_library PRIVATE ${CMAKE_Fortran_LIBRARIES})
