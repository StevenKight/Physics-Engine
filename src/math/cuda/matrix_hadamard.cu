/**
 * @file matrix_hadamard.cu
 * @brief CUDA implementation of element-wise matrix multiplication (Hadamard product).
 *
 * @author Steven Kight
 * @date 2026-04-10
 */

#include "matrix_core.h"

#include <iostream>

/**
 * @brief CUDA kernel for element-wise multiplication of two matrices.
 *
 * Each thread computes one element of the output matrix r.
 *
 * @param[in] a    Pointer to the first input matrix (device memory).
 * @param[in] b    Pointer to the second input matrix (device memory).
 * @param[out] r   Pointer to the output matrix (device memory).
 * @param[in] rows Number of rows in the matrices.
 * @param[in] cols Number of columns in the matrices.
 */
__global__ void matrix_hadamard_kernel(double *a, double *b, double *r, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < rows && col < cols)
        r[row * cols + col] = a[row * cols + col] * b[row * cols + col];
}

/**
 * @brief Host function to perform element-wise matrix multiplication using CUDA.
 *
 * @param[in] A  Pointer to the first input matrix (host memory).
 * @param[in] B  Pointer to the second input matrix (host memory).
 * @param[out] R Pointer to the result matrix (host memory).
 *
 * @note All matrices must have the same dimensions.
 */
extern "C" void matrix_hadamard_cuda(const Matrix *A, const Matrix *B, Matrix *R) {
    if (A->rows != B->rows || A->cols != B->cols) {
        std::cerr << "Matrix dimensions do not match for Hadamard product" << std::endl;
        return;
    }

    double *d_a, *d_b, *d_r;
    cudaMalloc((void **)&d_a, A->rows * A->cols * sizeof(double));
    cudaMalloc((void **)&d_b, B->rows * B->cols * sizeof(double));
    cudaMalloc((void **)&d_r, R->rows * R->cols * sizeof(double));

    cudaMemcpy(d_a, A->data, A->rows * A->cols * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, B->data, B->rows * B->cols * sizeof(double), cudaMemcpyHostToDevice);

    dim3 blockSize(16, 16);
    dim3 gridSize((A->cols + 15) / 16, (A->rows + 15) / 16);
    matrix_hadamard_kernel<<<gridSize, blockSize>>>(d_a, d_b, d_r, A->rows, A->cols);

    cudaMemcpy(R->data, d_r, R->rows * R->cols * sizeof(double), cudaMemcpyDeviceToHost);
    cudaFree(d_a); cudaFree(d_b); cudaFree(d_r);
}
