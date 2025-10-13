/**
 * @file matrix_scalar.cu
 * @brief CUDA implementations for matrix scalar operations.
 *
 * This file contains device and host functions for performing matrix scalar operations on the GPU.
 * Functions are designed for use in high-performance scientific computing and research applications.
 *
 * @author Steven Kight
 * @date 2025-10-09
 */

#include "matrix_core.h"


/**
 * @brief Kernel: multiply every element by a scalar.
 *
 * @param[in] a Device pointer to input matrix data
 * @param[in] scalar Scalar multiplier
 * @param[out] r Device pointer to output matrix data
 * @param[in] rows Number of rows
 * @param[in] cols Number of columns
 */
__global__ void matrix_scalar_multiply_kernel(float *a, float scalar, float *r, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows && col < cols)
        r[row * cols + col] = a[row * cols + col] * scalar;
}

/**
 * @brief Kernel: divide every element by a scalar.
 *
 * @param[in] a Device pointer to input matrix data
 * @param[in] scalar Scalar divisor
 * @param[out] r Device pointer to output matrix data
 * @param[in] rows Number of rows
 * @param[in] cols Number of columns
 */
__global__ void matrix_scalar_divide_kernel(float *a, float scalar, float *r, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows && col < cols)
        r[row * cols + col] = a[row * cols + col] / scalar;
}

/**
 * @brief Kernel: add a scalar to every element.
 *
 * @param[in] a Device pointer to input matrix data
 * @param[in] scalar Scalar to add
 * @param[out] r Device pointer to output matrix data
 * @param[in] rows Number of rows
 * @param[in] cols Number of columns
 */
__global__ void matrix_scalar_add_kernel(float *a, float scalar, float *r, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows && col < cols)
        r[row * cols + col] = a[row * cols + col] + scalar;
}

/**
 * @brief Kernel: subtract a scalar from every element.
 *
 * @param[in] a Device pointer to input matrix data
 * @param[in] scalar Scalar to subtract
 * @param[out] r Device pointer to output matrix data
 * @param[in] rows Number of rows
 * @param[in] cols Number of columns
 */
__global__ void matrix_scalar_subtract_kernel(float *a, float scalar, float *r, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows && col < cols)
        r[row * cols + col] = a[row * cols + col] - scalar;
}

/**
 * @brief Host wrapper: multiply matrix by scalar using the GPU.
 *
 * This function allocates device memory, copies the input matrix to the device,
 * launches the CUDA kernel, and copies the result back to the host.
 *
 * @param[in] m Pointer to input matrix (host memory)
 * @param[in] scalar Scalar multiplier
 * @param[out] r Pointer to result matrix (host memory)
 */
extern "C" void matrix_scalar_multiply_cuda(const Matrix *m, float scalar, Matrix *r) {
    float *d_a, *d_r;
    cudaMalloc((void **)&d_a, m->rows * m->cols * sizeof(float));
    cudaMalloc((void **)&d_r, m->rows * m->cols * sizeof(float));

    cudaMemcpy(d_a, m->data, m->rows * m->cols * sizeof(float), cudaMemcpyHostToDevice);
    dim3 blockSize(16, 16);
    dim3 gridSize((m->cols + 15) / 16, (m->rows + 15) / 16);
    matrix_scalar_multiply_kernel<<<gridSize, blockSize>>>(d_a, scalar, d_r, m->rows, m->cols);
    cudaMemcpy(r->data, d_r, m->rows * m->cols * sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_a); cudaFree(d_r);
}

/**
 * @brief Host wrapper: divide matrix by scalar using the GPU.
 *
 * This function allocates device memory, copies the input matrix to the device,
 * launches the CUDA kernel, and copies the result back to the host.
 *
 * @param[in] m Pointer to input matrix (host memory)
 * @param[in] scalar Scalar divisor
 * @param[out] r Pointer to result matrix (host memory)
 */
extern "C" void matrix_scalar_divide_cuda(const Matrix *m, float scalar, Matrix *r) {
    float *d_a, *d_r;
    cudaMalloc((void **)&d_a, m->rows * m->cols * sizeof(float));
    cudaMalloc((void **)&d_r, m->rows * m->cols * sizeof(float));

    cudaMemcpy(d_a, m->data, m->rows * m->cols * sizeof(float), cudaMemcpyHostToDevice);
    dim3 blockSize(16, 16);
    dim3 gridSize((m->cols + 15) / 16, (m->rows + 15) / 16);
    matrix_scalar_divide_kernel<<<gridSize, blockSize>>>(d_a, scalar, d_r, m->rows, m->cols);
    cudaMemcpy(r->data, d_r, m->rows * m->cols * sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_a); cudaFree(d_r);
}

/**
 * @brief Host wrapper: add scalar to matrix using the GPU.
 *
 * This function allocates device memory, copies the input matrix to the device,
 * launches the CUDA kernel, and copies the result back to the host.
 *
 * @param[in] m Pointer to input matrix (host memory)
 * @param[in] scalar Scalar to add
 * @param[out] r Pointer to result matrix (host memory)
 */
extern "C" void matrix_scalar_add_cuda(const Matrix *m, float scalar, Matrix *r) {
    float *d_a, *d_r;
    cudaMalloc((void **)&d_a, m->rows * m->cols * sizeof(float));
    cudaMalloc((void **)&d_r, m->rows * m->cols * sizeof(float));

    cudaMemcpy(d_a, m->data, m->rows * m->cols * sizeof(float), cudaMemcpyHostToDevice);
    dim3 blockSize(16, 16);
    dim3 gridSize((m->cols + 15) / 16, (m->rows + 15) / 16);
    matrix_scalar_add_kernel<<<gridSize, blockSize>>>(d_a, scalar, d_r, m->rows, m->cols);
    cudaMemcpy(r->data, d_r, m->rows * m->cols * sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_a); cudaFree(d_r);
}

/**
 * @brief Host wrapper: subtract scalar from matrix using the GPU.
 * This function allocates device memory, copies the input matrix to the device,
 * launches the CUDA kernel, and copies the result back to the host.
 *
 * @param[in] m Pointer to input matrix (host memory)
 * @param[in] scalar Scalar to subtract
 * @param[out] r Pointer to result matrix (host memory)
 */
extern "C" void matrix_scalar_subtract_cuda(const Matrix *m, float scalar, Matrix *r) {
    float *d_a, *d_r;
    cudaMalloc((void **)&d_a, m->rows * m->cols * sizeof(float));
    cudaMalloc((void **)&d_r, m->rows * m->cols * sizeof(float));

    cudaMemcpy(d_a, m->data, m->rows * m->cols * sizeof(float), cudaMemcpyHostToDevice);
    dim3 blockSize(16, 16);
    dim3 gridSize((m->cols + 15) / 16, (m->rows + 15) / 16);
    matrix_scalar_subtract_kernel<<<gridSize, blockSize>>>(d_a, scalar, d_r, m->rows, m->cols);
    cudaMemcpy(r->data, d_r, m->rows * m->cols * sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_a); cudaFree(d_r);
}
