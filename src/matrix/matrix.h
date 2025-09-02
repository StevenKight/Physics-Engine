// matrix.h
#ifndef MATRIX_OBJECT_H
#define MATRIX_OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

// Ensure compatibility for CUDA
#ifdef __CUDACC__
#define CUDA_HOSTDEV __host__ __device__
#else
#define CUDA_HOSTDEV
#endif

typedef struct {
    int rows;
    int cols;
    float *data;
} Matrix;

// Memory management
void initialize_matrix(Matrix *matrix, int rows, int cols);
void free_matrix(Matrix *matrix);
void print_matrix(const Matrix *matrix);
void fill_matrix(Matrix *matrix, float value);

// Matrix operations
void matrix_multiply(const Matrix *a, const Matrix *b, Matrix *result);
void matrix_add(const Matrix *a, const Matrix *b, Matrix *result);
void matrix_subtract(const Matrix *a, const Matrix *b, Matrix *result);

void matrix_scalar_multiply(const Matrix *matrix, float scalar, Matrix *result);
void matrix_scalar_add(const Matrix *matrix, float scalar, Matrix *result);
void matrix_scalar_subtract(const Matrix *matrix, float scalar, Matrix *result);

#ifdef __cplusplus
}
#endif

#endif // MATRIX_OBJECT_H