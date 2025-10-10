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
 * - Fortran backend: caller must pass double-precision arrays in column-major
 *   order. Dimensions are passed by address to match Fortran's ABI.
 * - CUDA backend: caller must pass pointers to `Matrix` objects (see
 *   `cuda_matrix.h`) containing row-major float data.
 * - The wrappers sometimes implement simple fallback logic (for example,
 *   scalar division for CUDA is implemented via multiplication by the
 *   reciprocal when a direct wrapper is not provided).
 * - These functions are not reentrant with respect to CUDA context setup;
 *   ensure the CUDA runtime is initialized in multithreaded environments.
 *
 * @author Steven Kight
 * @date 2025-10-10
 */

#include "matrix.h"

void matrix_add(const void* A, const void* B, void* C, int n, int m, bool use_gpu) {
    if (use_gpu) {
        // CUDA expects Matrix* structs
        matrix_add_cuda((const Matrix*)A, (const Matrix*)B, (Matrix*)C);
    } else {
        // Fortran expects double* arrays
        matrix_add_f((const double*)A, (const double*)B, (double*)C, &n, &m);
    }
}

void matrix_sub(const void* A, const void* B, void* C, int n, int m, bool use_gpu) {
    if (use_gpu) {
        matrix_subtract_cuda((const Matrix*)A, (const Matrix*)B, (Matrix*)C);
    } else {
        matrix_sub_f((const double*)A, (const double*)B, (double*)C, &n, &m);
    }
}

void matrix_mul(const void* A, const void* B, void* C, int n, int k, int m, bool use_gpu) {
    if (use_gpu) {
        matrix_multiply_cuda((const Matrix*)A, (const Matrix*)B, (Matrix*)C);
    } else {
        matrix_mul_f((const double*)A, (const double*)B, (double*)C, &n, &k, &m);
    }
}

void matrix_scalar_mul(const void* A, const void* scalar, void* C, int n, int m, bool use_gpu) {
    if (use_gpu) {
        matrix_scalar_multiply_cuda((const Matrix*)A, *(const float*)scalar, (Matrix*)C);
    } else {
        matrix_scalar_mul_f((const double*)A, (const double*)scalar, (double*)C, &n, &m);
    }
}

void matrix_scalar_div(const void* A, const void* scalar, void* C, int n, int m, bool use_gpu) {
    if (use_gpu) {
        // CUDA does not have a direct scalar division function; implement as multiplication by reciprocal
        float reciprocal = 1.0f / (*(const float*)scalar);
        matrix_scalar_multiply_cuda((const Matrix*)A, reciprocal, (Matrix*)C);
    } else {
        // Fortran does not have a direct scalar division function; implement as multiplication by reciprocal
        double reciprocal = 1.0 / (*(const double*)scalar);
        matrix_scalar_mul_f((const double*)A, &reciprocal, (double*)C, &n, &m);
    }
}

void matrix_scalar_add(const void* A, const void* scalar, void* C, int n, int m, bool use_gpu) {
    if (use_gpu) {
        // CUDA does not have a direct scalar addition function; implement manually
        Matrix* matA = (Matrix*)A;
        Matrix* matC = (Matrix*)C;
        float val = *(const float*)scalar;
        for (int i = 0; i < matA->rows * matA->cols; ++i) {
            matC->data[i] = matA->data[i] + val;
        }
    } else {
        // Fortran does not have a direct scalar addition function; implement manually
        const double* matA = (const double*)A;
        double* matC = (double*)C;
        double val = *(const double*)scalar;
        for (int i = 0; i < n * m; ++i) {
            matC[i] = matA[i] + val;
        }
    }
}

