// matrix.h
/**
 * @file matrix.h
 * @brief High-level matrix API that selects between Fortran and CUDA backends.
 *
 * This header exposes simple, backend-agnostic wrappers that call either
 * the Fortran implementations (column-major, double precision) or the CUDA
 * implementations (row-major, single precision) depending on the
 * `use_gpu` flag. Callers are responsible for providing correctly-typed
 * buffers and matching dimensions for the chosen backend.
 *
 * @author Steven Kight
 * @date 2025-10-10
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>

#include "cuda/cuda_matrix.h"
#include "fortran/fortran_matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Wrapper functions for dynamic backend selection.
 *
 * Notes on layout and types:
 * - Fortran backend expects double precision (double) arrays in column-major
 *   order and integer dimensions passed by value.
 * - CUDA backend expects single precision (float) arrays in row-major order
 *   stored in the Matrix struct defined in cuda_matrix.h.
 * - These wrappers do not perform type conversion. The caller must ensure
 *   the memory layout and element type match the backend chosen via
 *   `use_gpu`.
 */

/**
 * @brief Element-wise matrix addition wrapper.
 *
 * Chooses the GPU or CPU (Fortran) implementation based on `use_gpu`.
 *
 * @param A Pointer to the first input matrix. For Fortran backend this should
 *          be a pointer to double in column-major order. For CUDA backend,
 *          this should point to a `Matrix` (see cuda_matrix.h).
 * @param B Pointer to the second input matrix (same layout as A).
 * @param C Pointer to the output matrix storage. Must be pre-allocated and
 *          have appropriate size for the chosen backend.
 * @param n Number of rows.
 * @param m Number of columns.
 * @param use_gpu If true, use the CUDA backend; otherwise use Fortran.
 *
 * @note The function assumes the caller provides buffers matching the chosen
 *       backend. No runtime type conversions are performed.
 */
void matrix_add(const void *A, const void *B, void *C, int n, int m,
                bool use_gpu);

/**
 * @brief Element-wise matrix subtraction wrapper.
 *
 * @see matrix_add for parameter semantics and backend notes.
 */
void matrix_sub(const void *A, const void *B, void *C, int n, int m,
                bool use_gpu);

/**
 * @brief Matrix multiplication wrapper: C = A * B.
 *
 * @param A Pointer to left-hand matrix. Layout/type requirements same as in
 *          other wrappers.
 * @param B Pointer to right-hand matrix.
 * @param C Pointer to output matrix storage (pre-allocated).
 * @param n Number of rows of A and C.
 * @param k Shared dimension (cols of A / rows of B).
 * @param m Number of columns of B and C.
 * @param use_gpu If true, use the CUDA implementation; otherwise use Fortran.
 *
 * @note For the Fortran backend arrays are double precision and column-major.
 *       For the CUDA backend use the `Matrix` struct with float data in
 *       row-major layout. The caller must ensure A.cols == B.rows.
 */
void matrix_mul(const void *A, const void *B, void *C, int n, int k, int m,
                bool use_gpu);

/**
 * @brief Multiply each element by a scalar: C = A * scalar (wrapper).
 *
 * @param A Pointer to input matrix (see backend layout notes).
 * @param scalar Pointer to scalar value (Fortran expects double pointer; for
 * CUDA the scalar is a float value wrapped appropriately).
 * @param C Pointer to output matrix storage.
 * @param n Number of rows.
 * @param m Number of columns.
 * @param use_gpu Choose CUDA (true) or Fortran (false) backend.
 */
void matrix_scalar_mul(const void *A, const void *scalar, void *C, int n, int m,
                       bool use_gpu);

/**
 * @brief Divide each element by a scalar: C = A / scalar (wrapper).
 *
 * @param A Pointer to input matrix (see backend layout notes).
 * @param scalar Pointer to scalar value (Fortran expects double pointer; for
 * CUDA the scalar is a float value wrapped appropriately).
 * @param C Pointer to output matrix storage.
 * @param n Number of rows.
 * @param m Number of columns.
 * @param use_gpu Choose CUDA (true) or Fortran (false) backend.
 */
void matrix_scalar_div(const void *A, const void *scalar, void *C, int n, int m,
                       bool use_gpu);

/**
 * @brief Add scalar to each element: C = A + scalar (wrapper).
 *
 * @param A Pointer to input matrix (see backend layout notes).
 * @param scalar Pointer to scalar value (Fortran expects double pointer; for
 * CUDA the scalar is a float value wrapped appropriately).
 * @param C Pointer to output matrix storage.
 * @param n Number of rows.
 * @param m Number of columns.
 * @param use_gpu Choose CUDA (true) or Fortran (false) backend.
 */
void matrix_scalar_add(const void *A, const void *scalar, void *C, int n, int m,
                       bool use_gpu);

/**
 * @brief Subtract scalar from each element: C = A - scalar (wrapper).
 *
 * @param A Pointer to input matrix (see backend layout notes).
 * @param scalar Pointer to scalar value (Fortran expects double pointer; for
 * CUDA the scalar is a float value wrapped appropriately).
 * @param C Pointer to output matrix storage.
 * @param n Number of rows.
 * @param m Number of columns.
 * @param use_gpu Choose CUDA (true) or Fortran (false) backend.
 */
void matrix_scalar_sub(const void *A, const void *scalar, void *C, int n, int m,
                       bool use_gpu);

#ifdef __cplusplus
}
#endif

#endif // MATRIX_H
