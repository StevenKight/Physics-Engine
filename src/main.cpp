#include <stdio.h>
#include <vector>
#include <chrono>

#include <matrix.h>

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

void test_cuda_functions(const Matrix* a, const Matrix* b, Matrix* result) {
    test_and_run_function(
        "Matrix Multiplication", 
        matrix_multiply,
        a, b, result
    );
    printf("\n");

    test_and_run_function(
        "Matrix Addition",
        matrix_add,
        a, b, result
    );
    printf("\n");

    test_and_run_function(
        "Matrix Subtraction",
        matrix_subtract,
        a, b, result
    );
    printf("\n");
}

int main() {

    // Build test matrices
    Matrix a;
    Matrix b;
    Matrix result;

    initialize_matrix(&a, 50000, 50000);
    initialize_matrix(&b, 50000, 50000);
    fill_matrix(&a, 1.0f);
    fill_matrix(&b, 2.0f);

    // Test and run matrix operations
    printf("Testing CUDA functions:\n");
    test_cuda_functions(&a, &b, &result);
    printf("\n");

    // Free allocated memory
    free_matrix(&a);
    free_matrix(&b);

    // Setup test values
    float a_val = 1000000000.0f;
    float b_val = 2000000000.0f;
    float result_val = 0.0f;

    return 0;
}