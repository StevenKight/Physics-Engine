# Add your Fortran source files
set(IO_SOURCES
  parser.h parser.cpp
)

# Create a library from Fortran sources
add_library(io_lib ${IO_SOURCES})

# Include directories for the library
target_include_directories(io_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}  # Adds the current directory (which should contain utils.h) to the include path
)

# Include libraries if needed
target_link_libraries(io_lib PUBLIC state_lib)
