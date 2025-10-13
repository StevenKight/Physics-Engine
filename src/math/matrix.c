/**
 * @file matrix.c
 * @brief High-level matrix wrapper implementations that dispatch to backends.
 *
 * These functions select between the Fortran (CPU) and CUDA (GPU) backends
 * based on the `use_gpu` flag. They perform minimal parameter adaptation
 * (e.g., passing dimensions by address for Fortran bindings or casting
 * pointers to the CUDA `Matrix` struct) but do not perform type or layout
 * conversions for caller buffers.
 *
 * Expectations/Notes:
 * - Fortran backend: caller must pass double-precision arrays. 
 *   Dimensions are passed by address to match Fortran's ABI.
 * - CUDA backend: caller must pass pointers to `Matrix` objects (see
 *   `cuda_matrix.h`) containing row-major float data.
 * - These functions are not reentrant with respect to CUDA context setup;
 *   ensure the CUDA runtime is initialized in multithreaded environments.
 *
 * @author Steven Kight
 * @date 2025-10-10
 */

#include "matrix.h"
#include "cuda/cuda_matrix.h"

void matrix_mul(const void* A, const void* B, void* C, bool use_gpu) {
    if (use_gpu) {
        matrix_multiply_cuda((const Matrix*)A, (const Matrix*)B, (Matrix*)C);
        return;
    }

    /* For Fortran path we require Matrix* inputs so we can derive sizes. */
    const Matrix *ma = (const Matrix*)A;
    const Matrix *mb = (const Matrix*)B;
    Matrix *mc = (Matrix*)C;
    if (!ma || !mb || !mc) return; /* cannot infer dimensions */

    int rn = ma->rows;
    int rk = ma->cols;
    int rm = mb->cols;
    matrix_mul_f((const double*)ma->data, (const double*)mb->data, (double*)mc->data, &rn, &rk, &rm);
}

void matrix_add(const void* A, const void* B, void* C, bool use_gpu) {
    if (use_gpu) {
        matrix_add_cuda((const Matrix*)A, (const Matrix*)B, (Matrix*)C);
        return;
    }

    const Matrix *ma = (const Matrix*)A;
    const Matrix *mb = (const Matrix*)B;
    Matrix *mc = (Matrix*)C;
    if (!ma || !mb || !mc) return;
    int rn = ma->rows;
    int rm = ma->cols;
    matrix_add_f((const double*)ma->data, (const double*)mb->data, (double*)mc->data, &rn, &rm);
}

void matrix_sub(const void* A, const void* B, void* C, bool use_gpu) {
    if (use_gpu) {
        matrix_subtract_cuda((const Matrix*)A, (const Matrix*)B, (Matrix*)C);
        return;
    }

    const Matrix *ma = (const Matrix*)A;
    const Matrix *mb = (const Matrix*)B;
    Matrix *mc = (Matrix*)C;
    if (!ma || !mb || !mc) return;
    int rn = ma->rows;
    int rm = ma->cols;
    matrix_sub_f((const double*)ma->data, (const double*)mb->data, (double*)mc->data, &rn, &rm);
}

void matrix_scalar_mul(const void* A, const void* scalar, void* C, bool use_gpu) {
    if (use_gpu) {
        matrix_scalar_multiply_cuda((const Matrix*)A, *(const float*)scalar, (Matrix*)C);
        return;
    }

    const Matrix *ma = (const Matrix*)A;
    Matrix *mc = (Matrix*)C;
    if (!ma || !mc) return;
    int rn = ma->rows;
    int rm = ma->cols;
    matrix_scalar_mul_f((const double*)ma->data, (const double*)scalar, (double*)mc->data, &rn, &rm);
}

void matrix_scalar_div(const void* A, const void* scalar, void* C, bool use_gpu) {
    if (use_gpu) {
        matrix_scalar_divide_cuda((const Matrix*)A, *(const float*)scalar, (Matrix*)C);
        return;
    }

    const Matrix *ma = (const Matrix*)A;
    Matrix *mc = (Matrix*)C;
    if (!ma || !mc) return;
    int rn = ma->rows;
    int rm = ma->cols;
    matrix_scalar_div_f((const double*)ma->data, (const double*)scalar, (double*)mc->data, &rn, &rm);
}

void matrix_scalar_add(const void* A, const void* scalar, void* C, bool use_gpu) {
    if (use_gpu) {
        matrix_scalar_add_cuda((const Matrix*)A, *(const float*)scalar, (Matrix*)C);
        return;
    }

    const Matrix *ma = (const Matrix*)A;
    Matrix *mc = (Matrix*)C;
    if (!ma || !mc) return;
    int rn = ma->rows;
    int rm = ma->cols;
    matrix_scalar_add_f((const double*)ma->data, (const double*)scalar, (double*)mc->data, &rn, &rm);
}

void matrix_scalar_sub(const void* A, const void* scalar, void* C, bool use_gpu) {
    if (use_gpu) {
        /* CUDA implementation expects a Matrix pointer and a float scalar */
        matrix_scalar_subtract_cuda((const Matrix*)A, *(const float*)scalar, (Matrix*)C);
        return;
    }

    const Matrix *ma = (const Matrix*)A;
    Matrix *mc = (Matrix*)C;
    if (!ma || !mc) return;
    int rn = ma->rows;
    int rm = ma->cols;
    matrix_scalar_sub_f((const double*)ma->data, (const double*)scalar, (double*)mc->data, &rn, &rm);
}
