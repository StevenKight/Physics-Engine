/**
 * @file test_matrix_power.c
 * @brief Unit tests for matrix element-wise power operation
 *        across CPU/Fortran and GPU/CUDA backends.
 *
 * @author Steven Kight
 * @date 2026-04-09
 */

#include "matrix.h"
#include "test_runner.h"
#include <stdio.h>

/* --- power of 2 (square) --- */

static char *test_power_square_cpu() {
    int n = 2, m = 2;
    double A[4] = {1.0, 2.0, 3.0, 4.0};
    double power = 2.0;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_power(&A_mat, &power, &C_mat, false);

    printf("    CPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0],  1.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1],  4.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2],  9.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 16.0, 1e-9);
    return NULL;
}

static char *test_power_square_gpu() {
    int n = 2, m = 2;
    double A[4] = {1.0, 2.0, 3.0, 4.0};
    double power = 2.0;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_power(&A_mat, &power, &C_mat, true);

    printf("    GPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0],  1.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1],  4.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2],  9.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 16.0, 1e-9);
    return NULL;
}

/* --- power of 3 (cube) --- */

static char *test_power_cube_cpu() {
    int n = 2, m = 2;
    double A[4] = {1.0, 2.0, 3.0, 4.0};
    double power = 3.0;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_power(&A_mat, &power, &C_mat, false);

    printf("    CPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0],  1.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1],  8.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 27.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 64.0, 1e-9);
    return NULL;
}

static char *test_power_cube_gpu() {
    int n = 2, m = 2;
    double A[4] = {1.0, 2.0, 3.0, 4.0};
    double power = 3.0;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_power(&A_mat, &power, &C_mat, true);

    printf("    GPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0],  1.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1],  8.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 27.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 64.0, 1e-9);
    return NULL;
}

/* --- power of 0.5 (sqrt) --- */

static char *test_power_sqrt_cpu() {
    int n = 2, m = 2;
    double A[4] = {1.0, 4.0, 9.0, 16.0};
    double power = 0.5;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_power(&A_mat, &power, &C_mat, false);

    printf("    CPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], 1.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], 2.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 3.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 4.0, 1e-9);
    return NULL;
}

static char *test_power_sqrt_gpu() {
    int n = 2, m = 2;
    double A[4] = {1.0, 4.0, 9.0, 16.0};
    double power = 0.5;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_power(&A_mat, &power, &C_mat, true);

    printf("    GPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], 1.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], 2.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 3.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 4.0, 1e-9);
    return NULL;
}

static const TestCase tests[] = {
    {"power_square_cpu", test_power_square_cpu},
    {"power_square_gpu", test_power_square_gpu},
    {"power_cube_cpu",   test_power_cube_cpu},
    {"power_cube_gpu",   test_power_cube_gpu},
    {"power_sqrt_cpu",   test_power_sqrt_cpu},
    {"power_sqrt_gpu",   test_power_sqrt_gpu},
};

int main(void) {
    int failed = run_suite("Matrix Power Operation", tests,
                           sizeof(tests) / sizeof(tests[0]));
    return finish_suite(failed);
}
