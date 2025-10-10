

#include "math/matrix.h"

#include <chrono>
#include <iostream>
#include <vector>

// Helper for Fortran backend (double arrays)
void fill_matrix_double(std::vector<double> &mat, int n, int m, double value) {
    std::fill(mat.begin(), mat.end(), value);
}

// Helper for CUDA backend (Matrix struct)
Matrix create_and_fill_matrix(int rows, int cols, float value) {
    Matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.data = new float[rows * cols];
    for (int i = 0; i < rows * cols; ++i)
        mat.data[i] = value;
    return mat;
}

void free_matrix(Matrix &mat) {
    delete[] mat.data;
    mat.data = nullptr;
}

int main() {
    const int n = 1024, m = 1024;
    // Fortran test
    std::vector<double> A_f(n * m), B_f(n * m), C_f(n * m);
    fill_matrix_double(A_f, n, m, 1.0);
    fill_matrix_double(B_f, n, m, 2.0);

    auto start = std::chrono::high_resolution_clock::now();
    matrix_add(A_f.data(), B_f.data(), C_f.data(), n, m, false);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Fortran time: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms" << std::endl;

    // CUDA test
    Matrix A_c = create_and_fill_matrix(n, m, 1.0f);
    Matrix B_c = create_and_fill_matrix(n, m, 2.0f);
    Matrix C_c = create_and_fill_matrix(n, m, 0.0f);

    start = std::chrono::high_resolution_clock::now();
    matrix_add(&A_c, &B_c, &C_c, n, m, true);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "CUDA time: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms" << std::endl;

    free_matrix(A_c);
    free_matrix(B_c);
    free_matrix(C_c);

    return 0;
}