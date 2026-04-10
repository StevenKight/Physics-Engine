/**
 * @file test_matrix_scalar.c
 * @brief Unit tests for matrix scalar operations (mul, div, add, sub)
 *        across CPU/Fortran and GPU/CUDA backends.
 *
 * @author Steven Kight
 * @date 2026-04-09
 */

#include "matrix.h"
#include "test_runner.h"
#include <stdio.h>

/* --- multiply --- */

static char *test_scalar_mul_cpu() {
    int n = 2, m = 2;
    double A[4] = {1.0, 3.0, 2.0, 4.0};
    double scalar = 2.0;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_scalar_mul(&A_mat, &scalar, &C_mat, false);

    printf("    CPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], 2.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], 6.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 4.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 8.0, 1e-9);
    return NULL;
}

static char *test_scalar_mul_gpu() {
    int n = 2, m = 2;
    double A[4] = {1.0, 3.0, 2.0, 4.0};
    double scalar = 2.0;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_scalar_mul(&A_mat, &scalar, &C_mat, true);

    printf("    GPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], 2.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], 6.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 4.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 8.0, 1e-9);
    return NULL;
}

/* --- divide --- */

static char *test_scalar_div_cpu() {
    int n = 2, m = 2;
    double A[4] = {2.0, 6.0, 4.0, 8.0};
    double scalar = 2.0;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_scalar_div(&A_mat, &scalar, &C_mat, false);

    printf("    CPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], 1.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], 3.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 2.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 4.0, 1e-9);
    return NULL;
}

static char *test_scalar_div_gpu() {
    int n = 2, m = 2;
    double A[4] = {2.0, 6.0, 4.0, 8.0};
    double scalar = 2.0;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_scalar_div(&A_mat, &scalar, &C_mat, true);

    printf("    GPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], 1.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], 3.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 2.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 4.0, 1e-9);
    return NULL;
}

/* --- add --- */

static char *test_scalar_add_cpu() {
    int n = 2, m = 2;
    double A[4] = {1.0, 3.0, 2.0, 4.0};
    double scalar = 1.0;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_scalar_add(&A_mat, &scalar, &C_mat, false);

    printf("    CPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], 2.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], 4.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 3.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 5.0, 1e-9);
    return NULL;
}

static char *test_scalar_add_gpu() {
    int n = 2, m = 2;
    double A[4] = {1.0, 3.0, 2.0, 4.0};
    double scalar = 1.0;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_scalar_add(&A_mat, &scalar, &C_mat, true);

    printf("    GPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], 2.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], 4.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 3.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 5.0, 1e-9);
    return NULL;
}

/* --- subtract --- */

static char *test_scalar_sub_cpu() {
    int n = 2, m = 2;
    double A[4] = {1.0, 3.0, 2.0, 4.0};
    double scalar = 1.0;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_scalar_sub(&A_mat, &scalar, &C_mat, false);

    printf("    CPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], 0.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], 2.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 1.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 3.0, 1e-9);
    return NULL;
}

static char *test_scalar_sub_gpu() {
    int n = 2, m = 2;
    double A[4] = {1.0, 3.0, 2.0, 4.0};
    double scalar = 1.0;
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_mat = {n, m, A};
    Matrix C_mat = {n, m, C};

    matrix_scalar_sub(&A_mat, &scalar, &C_mat, true);

    printf("    GPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], 0.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], 2.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], 1.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], 3.0, 1e-9);
    return NULL;
}

static const TestCase tests[] = {
    {"scalar_mul_cpu", test_scalar_mul_cpu},
    {"scalar_mul_gpu", test_scalar_mul_gpu},
    {"scalar_div_cpu", test_scalar_div_cpu},
    {"scalar_div_gpu", test_scalar_div_gpu},
    {"scalar_add_cpu", test_scalar_add_cpu},
    {"scalar_add_gpu", test_scalar_add_gpu},
    {"scalar_sub_cpu", test_scalar_sub_cpu},
    {"scalar_sub_gpu", test_scalar_sub_gpu},
};

int main(void) {
    int failed = run_suite("Matrix Scalar Operations", tests,
                           sizeof(tests) / sizeof(tests[0]));
    return finish_suite(failed);
}
