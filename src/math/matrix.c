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

void matrix_mul(const void* A, const void* B, void* C, int n, int k, int m, bool use_gpu) {
    if (use_gpu) {
        matrix_multiply_cuda((const Matrix*)A, (const Matrix*)B, (Matrix*)C);
    } else {
        matrix_mul_f((const double*)A, (const double*)B, (double*)C, &n, &k, &m);
    }
}

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

void matrix_scalar_mul(const void* A, const void* scalar, void* C, int n, int m, bool use_gpu) {
    if (use_gpu) {
        matrix_scalar_multiply_cuda((const Matrix*)A, *(const float*)scalar, (Matrix*)C);
    } else {
        matrix_scalar_mul_f((const double*)A, (const double*)scalar, (double*)C, &n, &m);
    }
}

void matrix_scalar_div(const void* A, const void* scalar, void* C, int n, int m, bool use_gpu) {
    if (use_gpu) {
        matrix_scalar_divide_cuda((const Matrix*)A, *(const float*)scalar, (Matrix*)C);
    } else {
        matrix_scalar_div_f((const double*)A, (const double*)scalar, (double*)C, &n, &m);
    }
}

void matrix_scalar_add(const void* A, const void* scalar, void* C, int n, int m, bool use_gpu) {
    if (use_gpu) {
        matrix_scalar_add_cuda((const Matrix*)A, *(const float*)scalar, (Matrix*)C);
    } else {
        matrix_scalar_add_f((const double*)A, (const double*)scalar, (double*)C, &n, &m);
    }
}

void matrix_scalar_sub(const void* A, const void* scalar, void* C, int n, int m, bool use_gpu) {
    if (use_gpu) {
        /* CUDA implementation expects a Matrix pointer and a float scalar */
        matrix_scalar_subtract_cuda((const Matrix*)A, *(const float*)scalar, (Matrix*)C);
    } else {
        /* Fortran implementation expects double arrays and scalar, dimensions by address */
        matrix_scalar_sub_f((const double*)A, (const double*)scalar, (double*)C, &n, &m);
    }
}
