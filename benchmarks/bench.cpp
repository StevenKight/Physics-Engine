/**
 * @file bench.cpp
 * @brief Times gravity and full simulation ticks across body counts.
 *
 * Built three times (see CMakeLists.txt), once per gravity backend: always
 * Fortran, always CUDA, and the default router. The backend is chosen at
 * compile time through GRAVITY_GPU_THRESHOLD and named by BENCH_BACKEND.
 *
 * Scenarios:
 *   gravity    newtonian_gravity() alone on point masses
 *   sim_sparse sim_run() with random boxes spread out (few collisions)
 *   sim_dense  sim_run() with random boxes packed so neighbours overlap
 *
 * Each measurement is one discarded warm-up run followed by RUNS timed runs,
 * all from the same starting state. Rows are appended to the CSV given on the
 * command line.
 *
 * Usage: bench_<backend> <results.csv>
 *
 * @author Steven Kight
 * @date 2026-11-19
 */

#include "forces/gravity.h"
#include "sim.h"

#include <omp.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <functional>
#include <random>
#include <string>
#include <vector>

static const int RUNS = 10;
static const int SIM_TICKS = 10;
static const int MAX_N = 1 << 20;

// Stop going up in N once a single run takes longer than this.
static const double TIME_LIMIT_MS = 30000.0;

// newtonian_gravity() holds about 12 N×N double matrices at its peak.
static double gravity_bytes(int n) { return 12.0 * n * n * sizeof(double); }

static double available_bytes() {
    std::ifstream meminfo("/proc/meminfo");
    std::string key;
    double kb = 0.0;
    while (meminfo >> key >> kb) {
        if (key == "MemAvailable:")
            return kb * 1024.0;
        meminfo.ignore(256, '\n');
    }
    return 0.0;
}

static void set_box(PhysicsObject *o, double hx, double hy, double hz) {
    static const int faces[12][3] = {
        {0, 2, 1}, {0, 3, 2}, {4, 5, 6}, {4, 6, 7}, {0, 1, 5}, {0, 5, 4},
        {2, 3, 7}, {2, 7, 6}, {1, 2, 6}, {1, 6, 5}, {0, 4, 7}, {0, 7, 3},
    };
    const Vec3 verts[8] = {
        {-hx, -hy, -hz}, {hx, -hy, -hz}, {hx, hy, -hz}, {-hx, hy, -hz},
        {-hx, -hy, hz},  {hx, -hy, hz},  {hx, hy, hz},  {-hx, hy, hz},
    };
    std::copy(verts, verts + 8, o->local_verts);
    std::copy(&faces[0][0], &faces[0][0] + 36, &o->face_indices[0][0]);
    o->vertex_count = 8;
    o->face_count = 12;
}

// Point masses at random positions in a 1000 km cube.
static std::vector<PhysicsObject> make_points(int n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> pos(0.0, 1e6);
    std::uniform_real_distribution<double> mass(1e20, 1e22);

    std::vector<PhysicsObject> objects(n);
    for (auto &o : objects)
        object_init(&o, mass(rng), pos(rng), pos(rng), pos(rng));
    return objects;
}

// Random boxes on a cubic grid. A spacing well above the box size gives a
// sparse scene; a spacing below it makes every box overlap its neighbours.
static std::vector<PhysicsObject> make_boxes(int n, double spacing) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> half(0.25, 0.75);
    std::uniform_real_distribution<double> mass(1.0, 10.0);

    int side = 1;
    while (side * side * side < n)
        side++;

    std::vector<PhysicsObject> objects(n);
    for (int i = 0; i < n; i++) {
        double x = spacing * (i % side);
        double y = spacing * ((i / side) % side);
        double z = spacing * (i / (side * side));
        object_init(&objects[i], mass(rng), x, y, z);
        set_box(&objects[i], half(rng), half(rng), half(rng));
    }
    return objects;
}

struct Timing {
    double median, min, max;
};

// One warm-up run, then RUNS timed runs. reset() restores the starting state
// and is not timed.
static Timing time_runs(const std::function<void()> &reset,
                        const std::function<void()> &run) {
    reset();
    run();

    std::vector<double> ms;
    for (int r = 0; r < RUNS; r++) {
        reset();
        auto start = std::chrono::steady_clock::now();
        run();
        auto end = std::chrono::steady_clock::now();
        ms.push_back(
            std::chrono::duration<double, std::milli>(end - start).count());
    }

    std::sort(ms.begin(), ms.end());
    return {(ms[RUNS / 2 - 1] + ms[RUNS / 2]) / 2.0, ms.front(), ms.back()};
}

static FILE *csv;

static void write_row(const char *scenario, int threads, int n, int ticks,
                      const Timing &t, const char *note) {
    fprintf(csv, "%s,%s,%d,%d,%d,%d,%.4f,%.4f,%.4f,%s\n", scenario,
            BENCH_BACKEND, threads, n, ticks, RUNS, t.median, t.min, t.max,
            note);
    fflush(csv);
    printf("%-10s %-7s threads=%-2d n=%-7d median %.4f ms per tick %s\n",
           scenario, BENCH_BACKEND, threads, n, t.median, note);
}

using MakeFn = std::function<std::vector<PhysicsObject>(int)>;
using RunFn = std::function<void(std::vector<PhysicsObject> &)>;

// Runs one scenario for N = 8, 16, ... until memory or time runs out. Each
// run does `ticks` ticks of work; reported times are per tick. If `setup` is
// given, its median time is taken off every run before dividing by `ticks`.
static void sweep(const char *scenario, int threads, int ticks,
                  const MakeFn &make, const RunFn &run,
                  const RunFn &setup = nullptr) {
    omp_set_num_threads(threads);

    for (int n = 8; n <= MAX_N; n *= 2) {
        if (gravity_bytes(n) > 0.8 * available_bytes()) {
            write_row(scenario, threads, n, ticks, {0, 0, 0},
                      "skipped: not enough memory");
            return;
        }

        const std::vector<PhysicsObject> start = make(n);
        std::vector<PhysicsObject> objects;

        Timing t = time_runs([&] { objects = start; }, [&] { run(objects); });
        double longest_run = t.max;
        double overhead = 0.0;
        if (setup)
            overhead =
                time_runs([&] { objects = start; }, [&] { setup(objects); })
                    .median;
        t = {(t.median - overhead) / ticks, (t.min - overhead) / ticks,
             (t.max - overhead) / ticks};
        write_row(scenario, threads, n, ticks, t, "");

        if (longest_run > TIME_LIMIT_MS) {
            write_row(scenario, threads, n * 2, ticks, {0, 0, 0},
                      "skipped: previous run over time limit");
            return;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <results.csv>\n", argv[0]);
        return 1;
    }

    csv = fopen(argv[1], "a");
    if (!csv) {
        perror(argv[1]);
        return 1;
    }

    // Gravity has no OpenMP code, so it runs with one thread.
    sweep("gravity", 1, 1, make_points, [](std::vector<PhysicsObject> &o) {
        std::vector<Vec3> forces(o.size());
        newtonian_gravity(o.data(), (int)o.size(), forces.data());
    });

    auto sim_ticks = [](std::vector<PhysicsObject> &o) {
        sim_run(o.data(), (int)o.size(), 1.0, SIM_TICKS);
    };

    // sim_run() evaluates gravity once before its first tick. A run with zero
    // ticks does only that, so its time is subtracted to get a true per-tick
    // cost.
    auto sim_setup = [](std::vector<PhysicsObject> &o) {
        sim_run(o.data(), (int)o.size(), 1.0, 0);
    };

    // The full tick depends on the gravity router, so only that build runs
    // it, single threaded and with every core.
    if (std::string(BENCH_BACKEND) == "router") {
        int cores = omp_get_num_procs();
        for (int threads : {1, cores}) {
            sweep(
                "sim_sparse", threads, SIM_TICKS,
                [](int n) { return make_boxes(n, 3.0); }, sim_ticks, sim_setup);
            sweep(
                "sim_dense", threads, SIM_TICKS,
                [](int n) { return make_boxes(n, 1.0); }, sim_ticks, sim_setup);
        }
    }

    fclose(csv);
    return 0;
}
