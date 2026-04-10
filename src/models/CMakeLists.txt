# Collect all .c source files in this directory
file(GLOB MODELS_SOURCES "*.c")

# Create the models library
add_library(models_lib
    ${MODELS_SOURCES}
)

# Expose this directory for includes
target_include_directories(models_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
