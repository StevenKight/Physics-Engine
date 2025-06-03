# Add your Fortran source files
set(STATE_SOURCES
  object.h
)

# Create a library from Fortran sources
add_library(state_lib INTERFACE ${STATE_SOURCES})

# Include directories for the library
target_include_directories(state_lib INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}  # Adds the current directory (which should contain utils.h) to the include path
)
