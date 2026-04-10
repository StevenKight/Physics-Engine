/**
 * @file test_matrix_sub.c
 * @brief Unit tests for matrix subtraction (CPU/Fortran and GPU/CUDA backends).
 *
 * @author Steven Kight
 * @date 2026-04-09
 */

#include "matrix.h"
#include "test_runner.h"
#include <stdio.h>

static char *test_sub_cpu() {
    int n = 2, m = 2;
    double A[4] = {1.0,  3.0,  2.0,  4.0};
    double B[4] = {5.0,  8.0,  8.0, 11.0};
    double C[4] = {0.0,  0.0,  0.0,  0.0};

    Matrix A_mat = {n, m, A};
    Matrix B_mat = {n, m, B};
    Matrix C_mat = {n, m, C};

    matrix_sub(&A_mat, &B_mat, &C_mat, false);

    printf("    CPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], -4.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], -5.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], -6.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], -7.0, 1e-9);
    return NULL;
}

static char *test_sub_gpu() {
    int n = 2, m = 2;
    double A[4] = {1.0,  3.0,  2.0,  4.0};
    double B[4] = {5.0,  8.0,  8.0, 11.0};
    double C[4] = {0.0,  0.0,  0.0,  0.0};

    Matrix A_mat = {n, m, A};
    Matrix B_mat = {n, m, B};
    Matrix C_mat = {n, m, C};

    matrix_sub(&A_mat, &B_mat, &C_mat, true);

    printf("    GPU: %f %f %f %f\n", C[0], C[1], C[2], C[3]);
    mu_assert_double_eq("C[0] incorrect", C[0], -4.0, 1e-9);
    mu_assert_double_eq("C[1] incorrect", C[1], -5.0, 1e-9);
    mu_assert_double_eq("C[2] incorrect", C[2], -6.0, 1e-9);
    mu_assert_double_eq("C[3] incorrect", C[3], -7.0, 1e-9);
    return NULL;
}

static const TestCase tests[] = {
    {"sub_cpu", test_sub_cpu},
    {"sub_gpu", test_sub_gpu},
};

int main(void) {
    int failed = run_suite("Matrix Subtraction", tests,
                           sizeof(tests) / sizeof(tests[0]));
    return finish_suite(failed);
}
