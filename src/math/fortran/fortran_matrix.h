/**
 * @file fortran_matrix.h
 * @brief C header exposing Fortran matrix routines for interoperability.
 *
 * The Fortran sources in this directory implement basic linear algebra
 * operations in double precision (real(8)). These functions are bound to C
 * names using Fortran's bind(C) and are exposed here for linking from C/C++
 * research code. All arrays are assumed column-major when called from
 * Fortran; however the C interface here treats the arrays as plain pointers
 * and uses integer dimensions for interoperability.
 *
 * @author Steven Kight
 * @date 2025-10-09
 */

#ifndef MATRIX_FORTRAN_INTERFACE_H
#define MATRIX_FORTRAN_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Fortran interface expects double precision (real(8) in Fortran) */
/* C/Fortran interoperability signatures for matrix operations */
/**
 * @brief Element-wise addition: C = A + B
 * @param A Pointer to input matrix A (double*). Layout: column-major as in
 * Fortran.
 * @param B Pointer to input matrix B (double*). Same layout and dimensions as
 * A.
 * @param C Pointer to output matrix storage (double*). Must be allocated by
 * caller.
 * @param n Number of rows (pointer to int). Passed by address for Fortran
 * interoperability.
 * @param m Number of columns (pointer to int).
 */
void matrix_add_f(const double *A, const double *B, double *C, const int *n,
                  const int *m);

/**
 * @brief Element-wise subtraction: C = A - B
 * @param A Pointer to input matrix A (double*), column-major
 * @param B Pointer to input matrix B (double*), column-major
 * @param C Pointer to output matrix storage (double*)
 * @param n Pointer to number of rows
 * @param m Pointer to number of columns
 */
void matrix_sub_f(const double *A, const double *B, double *C, const int *n,
                  const int *m);

/**
 * @brief Matrix multiplication: C = A * B
 * @param A Pointer to left-hand matrix A (double*), dimensions n x k
 * @param B Pointer to right-hand matrix B (double*), dimensions k x m
 * @param C Pointer to output matrix storage (double*), dimensions n x m
 * @param n Pointer to number of rows of A and C
 * @param k Pointer to shared inner-dimension (cols of A / rows of B)
 * @param m Pointer to number of columns of B and C
 * layout.
 */
void matrix_mul_f(const double *A, const double *B, double *C, const int *n,
                  const int *k, const int *m);

/**
 * @brief Multiply each element of matrix A by scalar and store result in C: C =
 * A * scalar
 * @param A Pointer to input matrix A (double*), dimensions n x m
 * @param scalar Pointer to a scalar value (double*). Passed by address for
 * Fortran ABI.
 * @param C Pointer to output matrix storage (double*), dimensions n x m
 * @param n Pointer to number of rows
 * @param m Pointer to number of columns
 */
void matrix_scalar_mul_f(const double *A, const double *scalar, double *C,
                         const int *n, const int *m);

/**
 * @brief Divide each element of matrix A by scalar and store result in C: C = A
 * / scalar
 * @param A Pointer to input matrix A (double*), dimensions n x m
 * @param scalar Pointer to scalar divisor (double*)
 * @param C Pointer to output matrix storage (double*)
 * @param n Pointer to number of rows
 * @param m Pointer to number of columns
 *
 * @note No division-by-zero checks are performed in the Fortran implementation.
 */
void matrix_scalar_div_f(const double *A, const double *scalar, double *C,
                         const int *n, const int *m);

/**
 * @brief Add scalar to each element of matrix A: C = A + scalar
 * @param A Pointer to input matrix A (double*), dimensions n x m
 * @param scalar Pointer to scalar to add (double*)
 * @param C Pointer to output matrix storage (double*), dimensions n x m
 * @param n Pointer to number of rows
 * @param m Pointer to number of columns
 */
void matrix_scalar_add_f(const double *A, const double *scalar, double *C,
                         const int *n, const int *m);

/**
 * @brief Subtract scalar from each element of matrix A: C = A - scalar
 * @param A Pointer to input matrix A (double*), dimensions n x m
 * @param scalar Pointer to scalar to subtract (double*)
 * @param C Pointer to output matrix storage (double*), dimensions n x m
 * @param n Pointer to number of rows
 * @param m Pointer to number of columns
 */
void matrix_scalar_sub_f(const double *A, const double *scalar, double *C,
                         const int *n, const int *m);

#ifdef __cplusplus
}
#endif

#endif // MATRIX_FORTRAN_INTERFACE_H
