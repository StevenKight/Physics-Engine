/**
 * @file matrix_add.cu
 * @brief CUDA implementations for matrix addition operations.
 *
 * This file contains device and host functions for performing matrix addition on the GPU.
 *
 * @author Steven Kight
 * @date 2025-10-09
 */

#include "matrix_core.h"

#include <iostream>


/**
 * @brief CUDA kernel for element-wise addition of two matrices.
 *
 * Each thread computes one element of the output matrix r.
 *
 * @param[in] a Pointer to the first input matrix (device memory).
 * @param[in] b Pointer to the second input matrix (device memory).
 * @param[out] r Pointer to the output matrix (device memory).
 * @param[in] rows Number of rows in the matrices.
 * @param[in] cols Number of columns in the matrices.
 */
__global__ void matrix_add_kernel(float *a, float *b, float *r, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < rows && col < cols)
        r[row * cols + col] = a[row * cols + col] + b[row * cols + col];
}

/**
 * @brief Host function to perform matrix addition using CUDA.
 *
 * This function checks matrix dimensions, allocates device memory, copies data to the device,
 * launches the CUDA kernel for matrix addition, and copies the result back to the host.
 *
 * @param[in] A Pointer to the first input matrix (host memory).
 * @param[in] B Pointer to the second input matrix (host memory).
 * @param[out] R Pointer to the result matrix (host memory).
 *
 * @note All matrices must have the same dimensions. If not, the function prints an error and returns.
 */
extern "C" void matrix_add_cuda(const Matrix *A, const Matrix *B, Matrix *R) {
    if (A->rows != B->rows || A->cols != B->cols) {
        std::cerr << "Matrix dimensions do not match for addition" << std::endl;
        return;
    }

    float *d_a, *d_b, *d_r;
    cudaMalloc((void **)&d_a, A->rows * A->cols * sizeof(float));
    cudaMalloc((void **)&d_b, B->rows * B->cols * sizeof(float));
    cudaMalloc((void **)&d_r, R->rows * R->cols * sizeof(float));

    cudaMemcpy(d_a, A->data, A->rows * A->cols * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, B->data, B->rows * B->cols * sizeof(float), cudaMemcpyHostToDevice);

    dim3 blockSize(16, 16);
    dim3 gridSize((A->cols + 15) / 16, (A->rows + 15) / 16);
    matrix_add_kernel<<<gridSize, blockSize>>>(d_a, d_b, d_r, A->rows, A->cols);

    cudaMemcpy(R->data, d_r, R->rows * R->cols * sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_a); cudaFree(d_b); cudaFree(d_r);
}
