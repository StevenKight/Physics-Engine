

#include "math/matrix.h"

#include <chrono>
#include <iostream>

int main() {
    const int n = 2, m = 2;
    // Fortran test (uses double precision)
    double A[4] = {1.0, 3.0, 2.0, 4.0};
    double B[4] = {5.0, 7.0, 6.0, 8.0};
    double C[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_matrix_cpu = {n, m, (float *)(void *)A};
    Matrix B_matrix_cpu = {n, m, (float *)(void *)B};
    Matrix C_matrix_cpu = {n, m, (float *)(void *)C};

    auto start = std::chrono::high_resolution_clock::now();
    matrix_add(&A_matrix_cpu, &B_matrix_cpu, &C_matrix_cpu, false);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Fortran time: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms" << std::endl;
    std::cout << "C after CPU addition: " << C[0] << " " << C[1] << " " << C[2]
              << " " << C[3] << std::endl;

    // CUDA test
    float C_[4] = {0.0, 0.0, 0.0, 0.0};

    // Convert data to Matrix struct for CUDA
    Matrix C_two = {2, 2, C_};

    start = std::chrono::high_resolution_clock::now();
    matrix_add(&A, &B, &C_two, true);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "CUDA time: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms" << std::endl;
    std::cout << "C after GPU addition: " << C_[0] << " " << C_[1] << " "
              << C_[2] << " " << C_[3] << std::endl;

    return 0;
}