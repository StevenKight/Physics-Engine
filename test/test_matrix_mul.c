#include "matrix.h"
#include "minunit.h"
#include <stdio.h>

int tests_run = 0;

static char *test_matrix_mul_cpu() {
    int n = 2, k = 2, m = 2;

    double A[4] = {1.0, 3.0, 2.0, 4.0};
    double B[4] = {5.0, 7.0, 6.0, 8.0};
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_matrix_cpu = {n, k, (float*)(void*)A};
    Matrix B_matrix_cpu = {k, m, (float*)(void*)B};
    Matrix C_matrix_cpu = {n, m, (float*)(void*)C};

    matrix_mul(&A_matrix_cpu, &B_matrix_cpu, &C_matrix_cpu, false);

    printf("C after CPU multiplication: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], 23.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], 31.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 34.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 46.0, 1e-9);

    return 0;
}

static char *test_matrix_mul_gpu() {
    int n = 2, k = 2, m = 2;

    float A[4] = {1.0, 3.0, 2.0, 4.0};
    float B[4] = {5.0, 7.0, 6.0, 8.0};
    float C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_matrix = {2, 2, A};
    Matrix B_matrix = {2, 2, B};
    Matrix C_matrix = {2, 2, C};

    matrix_mul(&A_matrix, &B_matrix, &C_matrix, true); // use_gpu = true to call CUDA wrapper

    printf("C after GPU multiplication: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_float_eq("C[0] incorrect", C[0], 23.0, 1e-6);
    mu_assert_float_eq("C[1] incorrect", C[1], 31.0, 1e-6);
    mu_assert_float_eq("C[2] incorrect", C[2], 34.0, 1e-6);
    mu_assert_float_eq("C[3] incorrect", C[3], 46.0, 1e-6);

    return 0;
}

static char *all_tests() {
    mu_run_test(test_matrix_mul_cpu);
    mu_run_test(test_matrix_mul_gpu);
    return 0;
}

int main(void) {
    char *result = all_tests();
    if (result != 0) {
        printf("Test failed: %s\n", result);
    } else {
        printf("ALL TESTS PASSED (%d tests)\n", tests_run);
    }
    return result != 0;
}
