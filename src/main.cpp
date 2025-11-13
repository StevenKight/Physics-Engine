

#include "math/matrix.h"

#include <chrono>
#include <iostream>

int main() {
    const int n = 2, m = 2;
    // Fortran test (uses double precision)
    double A_One[4] = {1.0, 3.0, 2.0, 4.0};
    double B_One[4] = {5.0, 7.0, 6.0, 8.0};
    double C_One[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_matrix_cpu = {n, m, (float *)(void *)A_One};
    Matrix B_matrix_cpu = {n, m, (float *)(void *)B_One};
    Matrix C_matrix_cpu = {n, m, (float *)(void *)C_One};

    auto start = std::chrono::high_resolution_clock::now();
    matrix_add(&A_matrix_cpu, &B_matrix_cpu, &C_matrix_cpu, false);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Fortran time: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms" << std::endl;
    std::cout << "C after CPU addition: " << C_One[0] << " " << C_One[1] << " "
              << C_One[2] << " " << C_One[3] << std::endl;

    // CUDA test
    float A_Two[4] = {1.0f, 3.0f, 2.0f, 4.0f};
    float B_Two[4] = {5.0f, 7.0f, 6.0f, 8.0f};
    float C_Two[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    Matrix A_matrix_cuda = {n, m, A_Two};
    Matrix B_matrix_cuda = {n, m, B_Two};
    Matrix C_matrix_cuda = {n, m, C_Two};

    start = std::chrono::high_resolution_clock::now();
    matrix_add(&A_matrix_cuda, &B_matrix_cuda, &C_matrix_cuda, true);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "CUDA time: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms" << std::endl;
    std::cout << "C after GPU addition: " << C_Two[0] << " " << C_Two[1] << " "
              << C_Two[2] << " " << C_Two[3] << std::endl;

    return 0;
}