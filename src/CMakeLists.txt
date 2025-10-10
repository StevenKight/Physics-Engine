# Main application executable
add_executable(Physics main.cpp)

# Link to libraries defined in submodules
target_link_libraries(Physics PRIVATE
    math_lib
)

# If you need CUDA defines globally
target_compile_definitions(Physics PRIVATE -DUSE_CUDA)
