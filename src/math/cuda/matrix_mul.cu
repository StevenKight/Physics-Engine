/**
 * @file matrix_mul.cu
 * @brief CUDA implementations for matrix multiplication operations.
 *
 * This file contains device and host functions for performing matrix multiplication on the GPU.
 * Functions are designed for use in high-performance scientific computing and research applications.
 *
 * @author Steven Kight
 * @date 2025-10-09
 */
#include "matrix_core.h"

#include <iostream>


/**
 * @brief CUDA kernel performing matrix multiplication.
 *
 * Computes R = A * B using a straightforward inner product. Each thread
 * computes one element of the output matrix R.
 *
 * @param[in] a Device pointer to A data (rows_a x cols_a)
 * @param[in] rows_a Number of rows in A
 * @param[in] cols_a Number of cols in A
 * @param[in] b Device pointer to B data (rows_b x cols_b)
 * @param[in] rows_b Number of rows in B
 * @param[in] cols_b Number of cols in B
 * @param[out] r Device pointer to result data (rows_r x cols_r)
 * @param[in] rows_r Number of rows in result
 * @param[in] cols_r Number of cols in result
 */
__global__ void matrix_multiply_kernel(
    float *a, int rows_a, int cols_a, 
    float *b, int rows_b, int cols_b, 
    float *r, int rows_r, int cols_r
) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < rows_r && col < cols_r) {
        float sum = 0.0f;
        for (int k = 0; k < cols_a; k++) {
            sum += a[row * cols_a + k] * b[k * cols_b + col];
        }
        r[row * cols_r + col] = sum;
    }
}

/**
 * @brief Host wrapper that prepares device memory and launches the multiply kernel.
 *
 * This function validates input dimensions, allocates device buffers, copies
 * input data to device, launches the kernel, and retrieves the result.
 *
 * @param[in] A Pointer to left-hand matrix (host memory)
 * @param[in] B Pointer to right-hand matrix (host memory)
 * @param[out] R Pointer to result matrix (host memory)
 */
extern "C" void matrix_multiply_cuda(const Matrix *A, const Matrix *B, Matrix *R) {
    if (A->cols != B->rows) {
        std::cerr << "Matrix dimensions do not match for multiplication" << std::endl;
        return;
    }

    float *d_a, *d_b, *d_r;
    cudaMalloc((void **)&d_a, A->rows * A->cols * sizeof(float));
    cudaMalloc((void **)&d_b, B->rows * B->cols * sizeof(float));
    cudaMalloc((void **)&d_r, R->rows * R->cols * sizeof(float));

    cudaMemcpy(d_a, A->data, A->rows * A->cols * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, B->data, B->rows * B->cols * sizeof(float), cudaMemcpyHostToDevice);

    dim3 blockSize(16, 16);
    dim3 gridSize((R->cols + 15) / 16, (R->rows + 15) / 16);
    matrix_multiply_kernel<<<gridSize, blockSize>>>(d_a, A->rows, A->cols, d_b, B->rows, B->cols, d_r, R->rows, R->cols);

    cudaMemcpy(R->data, d_r, R->rows * R->cols * sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_a); cudaFree(d_b); cudaFree(d_r);
}
