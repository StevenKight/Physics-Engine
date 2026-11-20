/**
 * @file profile_gravity.cpp
 * @brief Runs one CUDA gravity call inside a profiler capture range.
 *
 * Does one warm-up call, then a single newtonian_gravity() call between
 * cudaProfilerStart() and cudaProfilerStop(), so Nsight Systems and Nsight
 * Compute see exactly one call. Built against the CUDA-forced gravity.c.
 *
 * Usage: profile_gravity <N>
 *
 * @author Steven Kight
 * @date 2026-11-20
 */

#include "forces/gravity.h"

#include <cuda_profiler_api.h>

#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <N>\n", argv[0]);
        return 1;
    }
    const int n = atoi(argv[1]);

    // Same point cloud as the gravity benchmark.
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> pos(0.0, 1e6);
    std::uniform_real_distribution<double> mass(1e20, 1e22);

    std::vector<PhysicsObject> objects(n);
    for (auto &o : objects)
        object_init(&o, mass(rng), pos(rng), pos(rng), pos(rng));
    std::vector<Vec3> forces(n);

    newtonian_gravity(objects.data(), n, forces.data());

    cudaProfilerStart();
    newtonian_gravity(objects.data(), n, forces.data());
    cudaProfilerStop();

    return 0;
}
