#include <chrono>
#include <iostream>
#include <stdio.h>
#include <vector>

#include <dummy.h>
#include <matrix.h>
#include <object.h>
#include <parser.h>

// Helper function to test and run a function that takes two matrices as params
void test_and_run_function(const char *function_name,
                           void (*function)(const Matrix *a, const Matrix *b,
                                            Matrix *result),
                           const Matrix *a, const Matrix *b, Matrix *result) {
    // Function Execution
    auto start = std::chrono::high_resolution_clock::now();
    function(a, b, result);
    auto end = std::chrono::high_resolution_clock::now();

    // Logging
    std::chrono::duration<float> duration = end - start;
    printf("%s time: %f seconds\n", function_name, duration.count());
}

// Helper function to test and run a function that takes two matrices as params
void test_and_run_scalar_function(
    const char *function_name,
    void (*function)(const Matrix *a, float scalar, Matrix *result),
    const Matrix *a, float scalar, Matrix *result) {
    // Function Execution
    auto start = std::chrono::high_resolution_clock::now();
    function(a, scalar, result);
    auto end = std::chrono::high_resolution_clock::now();

    // Logging
    std::chrono::duration<float> duration = end - start;
    printf("%s time: %f seconds\n", function_name, duration.count());
}

// Helper function to test CUDA functions
void test_cuda_functions(const Matrix *a, const Matrix *b, Matrix *result) {
    test_and_run_function("Matrix Multiplication", matrix_multiply, a, b,
                          result);

    test_and_run_function("Matrix Addition", matrix_add, a, b, result);

    test_and_run_function("Matrix Subtraction", matrix_subtract, a, b, result);

    test_and_run_scalar_function("Matrix Scalar Multiplication",
                                 matrix_scalar_multiply, a, 2.0f, result);

    test_and_run_scalar_function("Matrix Scalar Addition", matrix_scalar_add, a,
                                 2.0f, result);

    test_and_run_scalar_function("Matrix Scalar Subtraction",
                                 matrix_scalar_subtract, a, 2.0f, result);
}

int main() {

    // Build test matrices
    Matrix a;
    Matrix b;
    Matrix result;

    initialize_matrix(&a, 25000, 25000);
    initialize_matrix(&b, 25000, 25000);
    initialize_matrix(&result, 25000, 25000); // Allocate result matrix
    fill_matrix(&a, 1.0f);
    fill_matrix(&b, 2.0f);

    // Test and run matrix operations
    printf("Testing CUDA functions:\n");
    test_cuda_functions(&a, &b, &result);

    // Free allocated memory
    free_matrix(&a);
    free_matrix(&b);
    printf("\n");

    // Test Fortran integration
    printf("Testing Fortran integration:\n");
    say_hello();
    printf("\n");

    // Test file parsing
    printf("Testing file parsing:\n");
    auto objects = io::parse_phys_file(
        "/home/steven-kight/Documents/Physics/Physics-Engine/data/test.phys");

    for (const auto &obj : objects) {
        std::cout << "Object:\n"
                  << "  Position: " << obj.position.x << ", " << obj.position.y
                  << ", " << obj.position.z << "\n"
                  << "  Scale: " << obj.scale.x << ", " << obj.scale.y << ", "
                  << obj.scale.z << "\n"
                  << "  Mass: " << obj.mass << "\n"
                  << "  Velocity: " << obj.velocity.x << ", " << obj.velocity.y
                  << ", " << obj.velocity.z << "\n"
                  << "  Acceleration: " << obj.acceleration.x << ", "
                  << obj.acceleration.y << ", " << obj.acceleration.z << "\n";
    }
    printf("\n");

    if (objects.empty()) {
        std::cout << "No objects were parsed from the file!\n";
        return 1;
    }

    printf("Testing Verlet Integration on first object:\n");
    state::Object object = objects[0];
    state::Vec3 new_acceleration = {0, -9.81, 0}; // Example: gravity

    verlet_integration(object, new_acceleration, 1.0f); // 1 s timestep

    std::cout << "Object After 1 Timestep:\n"
              << "  Position: " << object.position.x << ", "
              << object.position.y << ", " << object.position.z << "\n"
              << "  Scale: " << object.scale.x << ", " << object.scale.y << ", "
              << object.scale.z << "\n"
              << "  Mass: " << object.mass << "\n"
              << "  Velocity: " << object.velocity.x << ", "
              << object.velocity.y << ", " << object.velocity.z << "\n"
              << "  Acceleration: " << object.acceleration.x << ", "
              << object.acceleration.y << ", " << object.acceleration.z << "\n";

    verlet_integration(object, new_acceleration, 1.0f); // 1 s timestep

    std::cout << "Object After 2 Timesteps:\n"
              << "  Position: " << object.position.x << ", "
              << object.position.y << ", " << object.position.z << "\n"
              << "  Scale: " << object.scale.x << ", " << object.scale.y << ", "
              << object.scale.z << "\n"
              << "  Mass: " << object.mass << "\n"
              << "  Velocity: " << object.velocity.x << ", "
              << object.velocity.y << ", " << object.velocity.z << "\n"
              << "  Acceleration: " << object.acceleration.x << ", "
              << object.acceleration.y << ", " << object.acceleration.z << "\n";

    return 0;
}