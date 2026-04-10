/**
 * @file matrix_power.cu
 * @brief CUDA implementation for element-wise matrix power.
 *
 * @author Steven Kight
 * @date 2026-04-09
 */

#include "matrix_core.h"

#include <math.h>


/**
 * @brief Kernel: raise every element of a matrix to a given power.
 *
 * @param[in]  a     Device pointer to input matrix data.
 * @param[in]  power The exponent to raise each element to.
 * @param[out] r     Device pointer to output matrix data.
 * @param[in]  rows  Number of rows.
 * @param[in]  cols  Number of columns.
 */
__global__ void matrix_power_kernel(double *a, double power, double *r, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows && col < cols) {
        int idx = row * cols + col;
        r[idx] = pow(a[idx], power);
    }
}

/**
 * @brief Host wrapper: raise each element of a matrix to a given power using the GPU.
 *
 * @param[in]  A     Pointer to input matrix (host memory).
 * @param[in]  power The exponent to raise each element to.
 * @param[out] R     Pointer to result matrix (host memory). Must be pre-allocated
 *                   with the same dimensions as @p A.
 */
extern "C" void matrix_power_cuda(const Matrix *A, double power, Matrix *R) {
    double *d_a, *d_r;
    cudaMalloc((void **)&d_a, A->rows * A->cols * sizeof(double));
    cudaMalloc((void **)&d_r, A->rows * A->cols * sizeof(double));

    cudaMemcpy(d_a, A->data, A->rows * A->cols * sizeof(double), cudaMemcpyHostToDevice);

    dim3 blockSize(16, 16);
    dim3 gridSize((A->cols + 15) / 16, (A->rows + 15) / 16);
    matrix_power_kernel<<<gridSize, blockSize>>>(d_a, power, d_r, A->rows, A->cols);

    cudaMemcpy(R->data, d_r, A->rows * A->cols * sizeof(double), cudaMemcpyDeviceToHost);
    cudaFree(d_a); cudaFree(d_r);
}
