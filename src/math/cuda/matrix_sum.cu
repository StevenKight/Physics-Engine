/**
 * @file matrix_sum.cu
 * @brief CUDA implementations for matrix row and column sum operations.
 *
 * This file contains device and host functions for summing rows and columns
 * of a matrix on the GPU.
 *
 * @author Steven Kight
 * @date 2026-04-10
 */

#include "matrix_core.h"

#include <iostream>


/**
 * @brief Kernel: sum each row of a matrix into a column vector.
 *
 * Each thread handles one row, accumulating all columns into a single output.
 *
 * @param[in]  a    Device pointer to input matrix data (rows x cols).
 * @param[out] r    Device pointer to output vector data (rows x 1).
 * @param[in]  rows Number of rows.
 * @param[in]  cols Number of columns.
 */
__global__ void matrix_row_sum_kernel(double *a, double *r, int rows, int cols) {
    int row = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows) {
        double sum = 0.0;
        for (int col = 0; col < cols; col++)
            sum += a[row * cols + col];
        r[row] = sum;
    }
}

/**
 * @brief Kernel: sum each column of a matrix into a row vector.
 *
 * Each thread handles one column, accumulating all rows into a single output.
 *
 * @param[in]  a    Device pointer to input matrix data (rows x cols).
 * @param[out] r    Device pointer to output vector data (1 x cols).
 * @param[in]  rows Number of rows.
 * @param[in]  cols Number of columns.
 */
__global__ void matrix_col_sum_kernel(double *a, double *r, int rows, int cols) {
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (col < cols) {
        double sum = 0.0;
        for (int row = 0; row < rows; row++)
            sum += a[row * cols + col];
        r[col] = sum;
    }
}

/**
 * @brief Host wrapper: sum each row of a matrix into a column vector using the GPU.
 *
 * @param[in]  A Pointer to input matrix (host memory, rows x cols).
 * @param[out] R Pointer to result matrix (host memory, rows x 1). Must be
 *               pre-allocated with rows equal to A->rows and cols equal to 1.
 */
extern "C" void matrix_row_sum_cuda(const Matrix *A, Matrix *R) {
    if (R->rows != A->rows || R->cols != 1) {
        std::cerr << "Output matrix must be (rows x 1) for row sum" << std::endl;
        return;
    }

    double *d_a, *d_r;
    cudaMalloc((void **)&d_a, A->rows * A->cols * sizeof(double));
    cudaMalloc((void **)&d_r, A->rows * sizeof(double));

    cudaMemcpy(d_a, A->data, A->rows * A->cols * sizeof(double), cudaMemcpyHostToDevice);

    int blockSize = 256;
    int gridSize = (A->rows + blockSize - 1) / blockSize;
    matrix_row_sum_kernel<<<gridSize, blockSize>>>(d_a, d_r, A->rows, A->cols);

    cudaMemcpy(R->data, d_r, A->rows * sizeof(double), cudaMemcpyDeviceToHost);
    cudaFree(d_a); cudaFree(d_r);
}

/**
 * @brief Host wrapper: sum each column of a matrix into a row vector using the GPU.
 *
 * @param[in]  A Pointer to input matrix (host memory, rows x cols).
 * @param[out] R Pointer to result matrix (host memory, 1 x cols). Must be
 *               pre-allocated with rows equal to 1 and cols equal to A->cols.
 */
extern "C" void matrix_col_sum_cuda(const Matrix *A, Matrix *R) {
    if (R->rows != 1 || R->cols != A->cols) {
        std::cerr << "Output matrix must be (1 x cols) for column sum" << std::endl;
        return;
    }

    double *d_a, *d_r;
    cudaMalloc((void **)&d_a, A->rows * A->cols * sizeof(double));
    cudaMalloc((void **)&d_r, A->cols * sizeof(double));

    cudaMemcpy(d_a, A->data, A->rows * A->cols * sizeof(double), cudaMemcpyHostToDevice);

    int blockSize = 256;
    int gridSize = (A->cols + blockSize - 1) / blockSize;
    matrix_col_sum_kernel<<<gridSize, blockSize>>>(d_a, d_r, A->rows, A->cols);

    cudaMemcpy(R->data, d_r, A->cols * sizeof(double), cudaMemcpyDeviceToHost);
    cudaFree(d_a); cudaFree(d_r);
}
