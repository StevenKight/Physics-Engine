/**
 * @file cuda_matrix.h
 * @brief Public C interface for CUDA-accelerated matrix operations.
 *
 * This header exposes C-callable wrappers around CUDA kernels. The functions
 * perform bounds checking on the dimensions where applicable and manage
 * copying data between host and device memory. Functions are named with a
 * _cuda suffix to make it explicit they use GPU execution under the hood.
 *
 * @author Steven Kight
 * @date 2025-10-09
 */

#ifndef MATRIX_CUDA_INTERFACE_H
#define MATRIX_CUDA_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

// Ensure compatibility for CUDA
#ifdef __CUDACC__
#define CUDA_HOSTDEV __host__ __device__
#else
#define CUDA_HOSTDEV
#endif

/**
 * @brief Simple matrix container used by CUDA wrappers.
 *
 * This struct represents a matrix stored in host memory using row-major
 * layout (C-style). The CUDA wrappers in this project copy from this
 * host buffer to device buffers before kernel execution and copy results back
 * into the provided `data` buffer.
 *
 * @param rows Number of rows in the matrix (positive integer).
 * @param cols Number of columns in the matrix (positive integer).
 * @param data Pointer to contiguous row-major float data in host memory.
 */
typedef struct {
    int rows;
    int cols;
    float *data;
} Matrix;

/* CUDA matrix operations (device and host callable) */
/**
 * @brief Element-wise addition: result = a + b
 * @param a Pointer to input matrix A (host memory, row-major)
 * @param b Pointer to input matrix B (host memory, row-major)
 * @param result Pointer to output matrix; must be pre-allocated with same
 * dimensions as A and B
 *
 * @note The function launches a CUDA kernel and copies data to/from the device.
 *       Both input matrices must have identical dimensions. Errors in CUDA
 * calls are not propagated via return value — check CUDA runtime for errors if
 * needed.
 */
void matrix_add_cuda(const Matrix *a, const Matrix *b, Matrix *result);

/**
 * @brief Element-wise subtraction: result = a - b
 * @param a Pointer to input matrix A (host memory, row-major)
 * @param b Pointer to input matrix B (host memory, row-major)
 * @param result Pointer to output matrix; must be pre-allocated with same
 * dimensions as A and B
 */
void matrix_subtract_cuda(const Matrix *a, const Matrix *b, Matrix *result);

/**
 * @brief Matrix multiplication: result = a * b
 * @param a Pointer to left-hand matrix A (host memory, row-major) with
 * dimensions rows_a x cols_a
 * @param b Pointer to right-hand matrix B (host memory, row-major) with
 * dimensions cols_a x cols_b
 * @param result Pointer to output matrix (host memory, row-major) with
 * dimensions rows_a x cols_b
 *
 * @note The number of columns of A must equal number of rows of B. This
 *       wrapper performs host-to-device copies, launches a kernel, and copies
 *       the result back. For large matrices, consider using tiled algorithms
 *       or cuBLAS for better performance.
 */
void matrix_multiply_cuda(const Matrix *a, const Matrix *b, Matrix *result);

/**
 * @brief Element-wise scalar multiplication: result = matrix * scalar
 * @param matrix Pointer to input matrix (host memory, row-major)
 * @param scalar Scalar multiplier
 * @param result Pointer to output matrix (host memory, row-major)
 */
void matrix_scalar_multiply_cuda(const Matrix *matrix, float scalar,
                                 Matrix *result);

/**
 * @brief Element-wise scalar division: result = matrix / scalar
 * @param matrix Pointer to input matrix (host memory, row-major)
 * @param scalar Scalar divisor
 * @param result Pointer to output matrix (host memory, row-major)
 *
 * @note No explicit division-by-zero checks are performed in the wrapper.
 */
void matrix_scalar_divide_cuda(const Matrix *matrix, float scalar,
                               Matrix *result);

/**
 * @brief Element-wise scalar addition: result = matrix + scalar
 * @param matrix Pointer to input matrix (host memory, row-major)
 * @param scalar Scalar to add
 * @param result Pointer to output matrix (host memory, row-major)
 */
void matrix_scalar_add_cuda(const Matrix *matrix, float scalar, Matrix *result);

/**
 * @brief Element-wise scalar subtraction: result = matrix - scalar
 * @param matrix Pointer to input matrix (host memory, row-major)
 * @param scalar Scalar to subtract
 * @param result Pointer to output matrix (host memory, row-major)
 */
void matrix_scalar_subtract_cuda(const Matrix *matrix, float scalar,
                                 Matrix *result);

#ifdef __cplusplus
}
#endif

#endif // MATRIX_CUDA_INTERFACE_H
