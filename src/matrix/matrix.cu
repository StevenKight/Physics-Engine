
#include <cuda_runtime.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

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

__global__ void matrix_add_kernel(float *a, float *b, float *r, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < rows && col < cols) {
        r[row * cols + col] = a[row * cols + col] + b[row * cols + col];
    }
}

__global__ void matrix_subtract_kernel(float *a, float *b, float *r, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < rows && col < cols) {
        r[row * cols + col] = a[row * cols + col] - b[row * cols + col];
    }
}

extern "C" void matrix_multiply(const Matrix *A, const Matrix *B, Matrix *R) {
    // Allocate matrix dimensions
    int *rows_a = (int *)&A->rows;
    int *cols_a = (int *)&A->cols;
    int *rows_b = (int *)&B->rows;
    int *cols_b = (int *)&B->cols;
    int *rows_r = (int *)&R->rows;
    int *cols_r = (int *)&R->cols;
    
    // Check if dimensions match
    if (*cols_a != *rows_b) {
        std::cerr << "Matrix dimensions do not match for multiplication" << std::endl;
        return;
    }

    // Allocate device memory
    float *d_a, *d_b, *d_r;
    cudaMalloc((void **)&d_a, A->rows * A->cols * sizeof(float));
    cudaMalloc((void **)&d_b, B->rows * B->cols * sizeof(float));
    cudaMalloc((void **)&d_r, R->rows * R->cols * sizeof(float));

    cudaMemcpy(d_a, A->data, (*rows_a) * (*cols_a) * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, B->data, (*rows_b) * (*cols_b) * sizeof(float), cudaMemcpyHostToDevice);

    // Calculate grid and block sizes
    dim3 blockSize(16, 16);
    dim3 gridSize((*cols_r + blockSize.x - 1) / blockSize.x, (*rows_r + blockSize.y - 1) / blockSize.y);

    // Launch kernel
    matrix_multiply_kernel<<<gridSize, blockSize>>>(d_a, *rows_a, *cols_a, d_b, *rows_b, *cols_b, d_r, *rows_r, *cols_r);

    // Copy result back to host
    cudaMemcpy(R->data, d_r, (*rows_r) * (*cols_r) * sizeof(float), cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_r);
}

extern "C" void matrix_add(const Matrix *A, const Matrix *B, Matrix *R) {
    // Allocate matrix dimensions
    int *rows_a = (int *)&A->rows;
    int *cols_a = (int *)&A->cols;
    int *rows_b = (int *)&B->rows;
    int *cols_b = (int *)&B->cols;
    int *rows_r = (int *)&R->rows;
    int *cols_r = (int *)&R->cols;

    // Check if dimensions match
    if (*rows_a != *rows_b || *cols_a != *cols_b) {
        std::cerr << "Matrix dimensions do not match for addition" << std::endl;
        return;
    }

    // Allocate device memory
    float *d_a, *d_b, *d_r;
    cudaMalloc((void **)&d_a, A->rows * A->cols * sizeof(float));
    cudaMalloc((void **)&d_b, B->rows * B->cols * sizeof(float));
    cudaMalloc((void **)&d_r, R->rows * R->cols * sizeof(float));

    cudaMemcpy(d_a, A->data, (*rows_a) * (*cols_a) * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, B->data, (*rows_b) * (*cols_b) * sizeof(float), cudaMemcpyHostToDevice);

    // Calculate grid and block sizes
    dim3 blockSize(16, 16);
    dim3 gridSize((*cols_a + blockSize.x - 1) / blockSize.x, (*rows_a + blockSize.y - 1) / blockSize.y);

    // Launch kernel
    matrix_add_kernel<<<gridSize, blockSize>>>(d_a, d_b, d_r, *rows_a, *cols_a);

    // Copy result back to host
    cudaMemcpy(R->data, d_r, (*rows_r) * (*cols_r) * sizeof(float), cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_r);
}

extern "C" void matrix_subtract(const Matrix *A, const Matrix *B, Matrix *R) {
    // Allocate matrix dimensions
    int *rows_a = (int *)&A->rows;
    int *cols_a = (int *)&A->cols;
    int *rows_b = (int *)&B->rows;
    int *cols_b = (int *)&B->cols;
    int *rows_r = (int *)&R->rows;
    int *cols_r = (int *)&R->cols;

    // Check if dimensions match
    if (*rows_a != *rows_b || *cols_a != *cols_b) {
        std::cerr << "Matrix dimensions do not match for subtraction" << std::endl;
        return;
    }

    // Allocate device memory
    float *d_a, *d_b, *d_r;
    cudaMalloc((void **)&d_a, A->rows * A->cols * sizeof(float));
    cudaMalloc((void **)&d_b, B->rows * B->cols * sizeof(float));
    cudaMalloc((void **)&d_r, R->rows * R->cols * sizeof(float));

    cudaMemcpy(d_a, A->data, (*rows_a) * (*cols_a) * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, B->data, (*rows_b) * (*cols_b) * sizeof(float), cudaMemcpyHostToDevice);

    // Calculate grid and block sizes
    dim3 blockSize(16, 16);
    dim3 gridSize((*cols_a + blockSize.x - 1) / blockSize.x, (*rows_a + blockSize.y - 1) / blockSize.y);

    // Launch kernel
    matrix_subtract_kernel<<<gridSize, blockSize>>>(d_a, d_b, d_r, *rows_a, *cols_a);

    // Copy result back to host
    cudaMemcpy(R->data, d_r, (*rows_r) * (*cols_r) * sizeof(float), cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_r);
}

// Initialize a matrix
extern "C" void initialize_matrix(Matrix* matrix, int rows, int cols) {
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = (float*)malloc(rows * cols * sizeof(float));
}

// Free matrix memory
extern "C" void free_matrix(Matrix* matrix) {
    if (matrix->data) {
        free(matrix->data);
        matrix->data = NULL;
    }
}

// Fill a matrix with a specific value
extern "C" void fill_matrix(Matrix* matrix, float value) {
    for (int i = 0; i < matrix->rows * matrix->cols; i++) {
        matrix->data[i] = value;
    }
}

// Print a matrix to the console
extern "C" void print_matrix(const Matrix* matrix) {
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            printf("%.2f ", matrix->data[i * matrix->cols + j]);
        }
        printf("\n");
    }
}