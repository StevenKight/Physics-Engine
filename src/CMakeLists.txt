# Define the list of source files
set(SOURCES
    main.cpp
)

# Add an executable target
add_executable(Physics ${SOURCES})

# Set Fortran flags (optional)
if (CMAKE_Fortran_COMPILER)
    target_link_libraries(Physics PRIVATE ${Fortran_LIBRARIES})
endif()

# CUDA-related settings (optional)
target_compile_options(Physics PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:-gencode arch=compute_70,code=sm_70>)

# Include directories if needed (e.g., CUDA includes)
target_include_directories(Physics PRIVATE ${CUDA_INCLUDE_DIRS})

# Set additional compile flags if necessary
target_compile_definitions(Physics PRIVATE -DUSE_CUDA)

# Link libraries if needed (e.g., CUDA libraries)
set(LOCAL_LIBS fortran_library matrix_library io_lib state_lib)
target_link_libraries(Physics PRIVATE ${LOCAL_LIBS})

# Include directories for the main program
target_include_directories(Physics PRIVATE
  ${CMAKE_SOURCE_DIR}/matrix  # Add path to cuda library
  ${CMAKE_SOURCE_DIR}/array  # Add path to fortran library
  ${CMAKE_SOURCE_DIR}/io      # Add path to io_lib, where file io functions are defined
  ${CMAKE_SOURCE_DIR}/state   # Add path to state_lib, where objects are defined
)
