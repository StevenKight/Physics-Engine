

#include "math/matrix.h"
#include "models/object.h"
#include "logic/sim.h"

#include <chrono>
#include <iostream>

static void test_matrix() {
    const int n = 2, m = 2;

    // Fortran (CPU) test
    double A_One[4] = {1.0, 3.0, 2.0, 4.0};
    double B_One[4] = {5.0, 7.0, 6.0, 8.0};
    double C_One[4] = {0.0, 0.0, 0.0, 0.0};

    Matrix A_matrix_cpu = {n, m, A_One};
    Matrix B_matrix_cpu = {n, m, B_One};
    Matrix C_matrix_cpu = {n, m, C_One};

    auto start = std::chrono::high_resolution_clock::now();
    matrix_add(&A_matrix_cpu, &B_matrix_cpu, &C_matrix_cpu, false);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Fortran time: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms" << std::endl;
    std::cout << "C after CPU addition: " << C_One[0] << " " << C_One[1] << " "
              << C_One[2] << " " << C_One[3] << std::endl;

    // CUDA (GPU) test
    double A_Two[4] = {1.0, 3.0, 2.0, 4.0};
    double B_Two[4] = {5.0, 7.0, 6.0, 8.0};
    double C_Two[4] = {0.0, 0.0, 0.0, 0.0};

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
}

static void test_sim() {
    // Two bodies: Sun-like and Earth-like, separated by ~1 AU on the x-axis.
    PhysicsObject objects[2];
    object_init(&objects[0], 1.989e30,  0.0,         0.0, 0.0); // Sun
    object_init(&objects[1], 5.972e24,  1.496e11,    0.0, 0.0); // Earth

    // Give Earth an initial orbital velocity (approx. 29,780 m/s along y).
    objects[1].velocity.y = 29780.0;

    const double time_step = 3600.0;   // 1 hour in seconds
    const int    num_steps = 24;       // 1 simulated day

    auto start = std::chrono::high_resolution_clock::now();
    sim_run(objects, 2, time_step, num_steps);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Sim time: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms" << std::endl;
    std::cout << "Earth position after " << num_steps << " steps: ("
              << objects[1].position.x << ", "
              << objects[1].position.y << ", "
              << objects[1].position.z << ")" << std::endl;
}

int main() {
    test_matrix();
    test_sim();
    return 0;
}
