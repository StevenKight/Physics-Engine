/**
 * @file test_newtonian_gravity.c
 * @brief Unit tests for newtonian_gravity().
 *
 * Tests cover: single-body (zero force), two-body axis-aligned force magnitude,
 * Newton's third law (action-reaction symmetry), three-body collinear superposition,
 * and diagonal force direction via a 3-4-5 right triangle.
 *
 * All tests run through the public newtonian_gravity() entry point, which
 * automatically selects the Fortran (CPU) backend for small N (since these
 * tests compile without USE_CUDA).
 *
 * @author Steven Kight
 * @date 2026-04-10
 */

#include "forces/gravity.h"
#include "test_runner.h"
#include <stdio.h>

static const double G = 6.67430e-11;

/**
 * A single body has no partners — net force must be zero in all components.
 */
static char *test_single_body() {
    PhysicsObject obj = { .mass = 1.0, .position = {0.0, 0.0, 0.0} };
    Vec3 force = {0.0, 0.0, 0.0};

    newtonian_gravity(&obj, 1, &force);

    mu_assert_double_eq("single body: Fx != 0", force.x, 0.0, 1e-30);
    mu_assert_double_eq("single body: Fy != 0", force.y, 0.0, 1e-30);
    mu_assert_double_eq("single body: Fz != 0", force.z, 0.0, 1e-30);
    return NULL;
}

/**
 * Two equal unit masses separated by 1 m along the x-axis.
 *
 * Expected:  F = G * m1 * m2 / r^2 = G * 1 * 1 / 1 = G
 *   forces[0] = (+G,  0, 0)   (body 0 pulled toward body 1)
 *   forces[1] = (-G,  0, 0)   (body 1 pulled toward body 0)
 *
 * All intermediate values are exactly representable, so tolerance is tight.
 */
static char *test_two_body_axis() {
    PhysicsObject objects[2] = {
        { .mass = 1.0, .position = {0.0, 0.0, 0.0} },
        { .mass = 1.0, .position = {1.0, 0.0, 0.0} },
    };
    Vec3 forces[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

    newtonian_gravity(objects, 2, forces);

    printf("    body 0: Fx=%.6e  Fy=%.6e  Fz=%.6e\n",
           forces[0].x, forces[0].y, forces[0].z);
    printf("    body 1: Fx=%.6e  Fy=%.6e  Fz=%.6e\n",
           forces[1].x, forces[1].y, forces[1].z);

    mu_assert_double_eq("body 0: Fx != +G",  forces[0].x,  G,   1e-20);
    mu_assert_double_eq("body 0: Fy != 0",   forces[0].y,  0.0, 1e-30);
    mu_assert_double_eq("body 0: Fz != 0",   forces[0].z,  0.0, 1e-30);
    mu_assert_double_eq("body 1: Fx != -G",  forces[1].x, -G,   1e-20);
    mu_assert_double_eq("body 1: Fy != 0",   forces[1].y,  0.0, 1e-30);
    mu_assert_double_eq("body 1: Fz != 0",   forces[1].z,  0.0, 1e-30);
    return NULL;
}

/**
 * Newton's third law: for any two-body system, forces[0] == -forces[1].
 *
 * Uses unequal masses and a diagonal separation to exercise the general case.
 */
static char *test_newton_third_law() {
    PhysicsObject objects[2] = {
        { .mass = 2.0, .position = {0.0, 0.0, 0.0} },
        { .mass = 5.0, .position = {3.0, 4.0, 0.0} }, // r = 5
    };
    Vec3 forces[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

    newtonian_gravity(objects, 2, forces);

    printf("    body 0: Fx=%.6e  Fy=%.6e  Fz=%.6e\n",
           forces[0].x, forces[0].y, forces[0].z);
    printf("    body 1: Fx=%.6e  Fy=%.6e  Fz=%.6e\n",
           forces[1].x, forces[1].y, forces[1].z);

    mu_assert_double_eq("3rd law: Fx not equal-opposite", forces[0].x, -forces[1].x, 1e-30);
    mu_assert_double_eq("3rd law: Fy not equal-opposite", forces[0].y, -forces[1].y, 1e-30);
    mu_assert_double_eq("3rd law: Fz not equal-opposite", forces[0].z, -forces[1].z, 1e-30);
    return NULL;
}

/**
 * Three equal unit masses placed at x = -1, 0, +1 (collinear on x-axis).
 *
 * Body 0 (leftmost) is pulled right by both body 1 (r=1) and body 2 (r=2):
 *   F_net = G/1 + G/4 = 5G/4
 *
 * Body 2 (rightmost) is the mirror image: F_net = -5G/4.
 *
 * Body 1 (middle) receives equal and opposite pulls from body 0 and body 2:
 *   F_net = 0.
 */
static char *test_three_body_collinear() {
    PhysicsObject objects[3] = {
        { .mass = 1.0, .position = {-1.0, 0.0, 0.0} },
        { .mass = 1.0, .position = { 0.0, 0.0, 0.0} },
        { .mass = 1.0, .position = { 1.0, 0.0, 0.0} },
    };
    Vec3 forces[3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

    newtonian_gravity(objects, 3, forces);

    double F_outer = G * 1.0 / 1.0 + G * 1.0 / 4.0; // G + G/4 = 5G/4

    printf("    body 0: Fx=%.6e  (expected %.6e)\n", forces[0].x,  F_outer);
    printf("    body 1: Fx=%.6e  (expected 0)\n",    forces[1].x);
    printf("    body 2: Fx=%.6e  (expected %.6e)\n", forces[2].x, -F_outer);

    mu_assert_double_eq("body 0: Fx != +5G/4", forces[0].x,  F_outer, 1e-20);
    mu_assert_double_eq("body 0: Fy != 0",     forces[0].y,  0.0,     1e-30);
    mu_assert_double_eq("body 0: Fz != 0",     forces[0].z,  0.0,     1e-30);

    mu_assert_double_eq("body 1: Fx != 0",     forces[1].x,  0.0,     1e-20);
    mu_assert_double_eq("body 1: Fy != 0",     forces[1].y,  0.0,     1e-30);
    mu_assert_double_eq("body 1: Fz != 0",     forces[1].z,  0.0,     1e-30);

    mu_assert_double_eq("body 2: Fx != -5G/4", forces[2].x, -F_outer, 1e-20);
    mu_assert_double_eq("body 2: Fy != 0",     forces[2].y,  0.0,     1e-30);
    mu_assert_double_eq("body 2: Fz != 0",     forces[2].z,  0.0,     1e-30);
    return NULL;
}

/**
 * Two equal unit masses with a diagonal 3-4-5 separation (r = 5).
 *
 * Verifies that both the force magnitude and direction are correct:
 *   F_scalar = G * 1 * 1 / 5^2 = G / 25
 *   unit dir = (3/5, 4/5, 0)
 *   forces[0] = ( 3G/125,  4G/125, 0)
 *   forces[1] = (-3G/125, -4G/125, 0)
 *
 * r = sqrt(25) = 5 exactly, so only G/25 and the 3/5, 4/5 products
 * introduce rounding — relative error is a few ULPs.
 */
static char *test_two_body_diagonal() {
    PhysicsObject objects[2] = {
        { .mass = 1.0, .position = {0.0, 0.0, 0.0} },
        { .mass = 1.0, .position = {3.0, 4.0, 0.0} },
    };
    Vec3 forces[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

    newtonian_gravity(objects, 2, forces);

    double F = G / 25.0;          // G * m1*m2 / r^2
    double Fx = F * (3.0 / 5.0); // F scaled by unit-vector x-component
    double Fy = F * (4.0 / 5.0); // F scaled by unit-vector y-component

    printf("    body 0: Fx=%.6e  Fy=%.6e  Fz=%.6e\n",
           forces[0].x, forces[0].y, forces[0].z);
    printf("    body 1: Fx=%.6e  Fy=%.6e  Fz=%.6e\n",
           forces[1].x, forces[1].y, forces[1].z);

    mu_assert_double_eq("body 0: Fx wrong",  forces[0].x,  Fx,  1e-22);
    mu_assert_double_eq("body 0: Fy wrong",  forces[0].y,  Fy,  1e-22);
    mu_assert_double_eq("body 0: Fz != 0",   forces[0].z,  0.0, 1e-30);
    mu_assert_double_eq("body 1: Fx wrong",  forces[1].x, -Fx,  1e-22);
    mu_assert_double_eq("body 1: Fy wrong",  forces[1].y, -Fy,  1e-22);
    mu_assert_double_eq("body 1: Fz != 0",   forces[1].z,  0.0, 1e-30);
    return NULL;
}

static const TestCase tests[] = {
    {"single_body",          test_single_body},
    {"two_body_axis",        test_two_body_axis},
    {"newton_third_law",     test_newton_third_law},
    {"three_body_collinear", test_three_body_collinear},
    {"two_body_diagonal",    test_two_body_diagonal},
};

int main(void) {
    int failed = run_suite("Newtonian Gravity", tests,
                           sizeof(tests) / sizeof(tests[0]));
    return finish_suite(failed);
}
