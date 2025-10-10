#include "matrix_core.h"

#include <iostream>


/**
 * @brief Kernel: element-wise subtraction R = A - B
 * 
 * This kernel computes the element-wise subtraction of two matrices A and B,
 * storing the result in matrix R. Each thread calculates one element of the
 * output matrix.
 *
 * @param[in] a Device pointer to first input matrix data
 * @param[in] b Device pointer to second input matrix data
 * @param[out] r Device pointer to output matrix data
 * @param[in] rows Number of rows
 * @param[in] cols Number of columns
 */
__global__ void matrix_subtract_kernel(float *a, float *b, float *r, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < rows && col < cols)
        r[row * cols + col] = a[row * cols + col] - b[row * cols + col];
}

/**
 * @brief Host wrapper for matrix subtraction using the GPU.
 *
 * Validates dimensions, copies inputs to the device, launches the kernel,
 * and copies back the result.
 *
 * @param[in] A Pointer to first input matrix (host memory)
 * @param[in] B Pointer to second input matrix (host memory)
 * @param[out] R Pointer to result matrix (host memory)
 *
 * @note All matrices must have the same dimensions. If not, the function prints an error and returns.
 */
extern "C" void matrix_subtract_cuda(const Matrix *A, const Matrix *B, Matrix *R) {
    if (A->rows != B->rows || A->cols != B->cols) {
        std::cerr << "Matrix dimensions do not match for subtraction" << std::endl;
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
    matrix_subtract_kernel<<<gridSize, blockSize>>>(d_a, d_b, d_r, A->rows, A->cols);

    cudaMemcpy(R->data, d_r, R->rows * R->cols * sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_a); cudaFree(d_b); cudaFree(d_r);
}
