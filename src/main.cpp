#include <stdio.h>
#include <vector>
#include <chrono>
#include <iostream>

#include <parser.h>
#include <matrix.h>
#include <dummy.h>

// Helper function to test and run a function
void test_and_run_function(
    char* function_name,
    void (*function)(const Matrix* a, const Matrix* b, Matrix* result),
    const Matrix* a, const Matrix* b, Matrix* result
) {
    // Function Execution
    auto start = std::chrono::high_resolution_clock::now();
    function(a, b, result);
    auto end = std::chrono::high_resolution_clock::now();
    
    // Logging
    std::chrono::duration<float> duration = end - start;
    printf("%s time: %f seconds\n", function_name, duration.count());
}

// Helper function to test CUDA functions
void test_cuda_functions(const Matrix* a, const Matrix* b, Matrix* result) {
    test_and_run_function(
        "Matrix Multiplication", 
        matrix_multiply,
        a, b, result
    );

    test_and_run_function(
        "Matrix Addition",
        matrix_add,
        a, b, result
    );

    test_and_run_function(
        "Matrix Subtraction",
        matrix_subtract,
        a, b, result
    );
}

int main() {

    // Build test matrices
    Matrix a;
    Matrix b;
    Matrix result;

    initialize_matrix(&a, 25000, 25000);
    initialize_matrix(&b, 25000, 25000);
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
    auto objects = io::parse_phys_file("/home/steven-kight/Documents/Physics/Physics-Engine/data/test.phys");

    for (const auto& obj : objects) {
        std::cout << "Object:\n"
                  << "  Center: " << obj.center.x << ", " << obj.center.y << ", " << obj.center.z << "\n"
                  << "  Scale: "  << obj.scale.x << ", " << obj.scale.y << ", " << obj.scale.z << "\n"
                  << "  Mass: "   << obj.mass << "\n"
                  << "  Velocity: " << obj.velocity.x << ", " << obj.velocity.y << ", " << obj.velocity.z << "\n";
    }

    return 0;
}