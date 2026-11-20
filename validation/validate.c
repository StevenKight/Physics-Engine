/**
 * @file validate.c
 * @brief Checks the simulation against analytic physics.
 *
 * Every case runs through the public sim_run() entry point. Prints a markdown
 * results table to stdout and exits non-zero if any case fails, so it can be
 * used both to regenerate validation/RESULTS.md and as a ctest.
 *
 * Usage: OMP_NUM_THREADS=1 ./build/validation/validate > validation/RESULTS.md
 *
 * @author Steven Kight
 * @date 2026-11-19
 */

#include "sim.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define G_NEWTON 6.67430e-11 /* Same value as gravity.c */

static int failures = 0;

static void report(const char *name, const char *metric, double tolerance,
                   double measured) {
    int pass = fabs(measured) <= tolerance;
    if (!pass)
        failures++;
    printf("| %s | %s | %.1e | %.3e | %s |\n", name, metric, tolerance,
           measured, pass ? "pass" : "FAIL");
}

/* ------------------------------------------------------------------ */
/* Two-body helpers                                                     */
/* ------------------------------------------------------------------ */

static const double M_SUN = 1.989e30;
static const double M_EARTH = 5.972e24;
static const double AU = 1.496e11;

/*
 * Sets up a two-body orbit with semi-major axis a and eccentricity e, starting
 * at periapsis on the +x axis and moving in +y. Velocities are in the centre
 * of mass frame so the system doesn't drift.
 */
static void two_body_init(PhysicsObject o[2], double a, double e) {
    double mu = G_NEWTON * (M_SUN + M_EARTH);
    double r_peri = a * (1.0 - e);
    double v_rel = sqrt(mu * (1.0 + e) / r_peri);
    double frac_sun = M_EARTH / (M_SUN + M_EARTH);

    memset(o, 0, 2 * sizeof(PhysicsObject));
    object_init(&o[0], M_SUN, -r_peri * frac_sun, 0.0, 0.0);
    object_init(&o[1], M_EARTH, r_peri * (1.0 - frac_sun), 0.0, 0.0);
    o[0].velocity.y = -v_rel * frac_sun;
    o[1].velocity.y = v_rel * (1.0 - frac_sun);
}

static double two_body_period(double a) {
    return 2.0 * M_PI * sqrt(a * a * a / (G_NEWTON * (M_SUN + M_EARTH)));
}

static double total_energy(const PhysicsObject o[2]) {
    double r = vec3_magnitude(vec3_sub(o[1].position, o[0].position));
    double ke = 0.0;
    for (int i = 0; i < 2; i++)
        ke += 0.5 * o[i].mass * vec3_dot(o[i].velocity, o[i].velocity);
    return ke - G_NEWTON * o[0].mass * o[1].mass / r;
}

static double angular_momentum_z(const PhysicsObject o[2]) {
    double lz = 0.0;
    for (int i = 0; i < 2; i++)
        lz += o[i].mass * vec3_cross(o[i].position, o[i].velocity).z;
    return lz;
}

/* ------------------------------------------------------------------ */
/* Cases                                                                */
/* ------------------------------------------------------------------ */

/*
 * A single body with no forces must move in a straight line at constant
 * velocity: x(t) = x0 + v*t.
 */
static void free_flight(void) {
    PhysicsObject o;
    memset(&o, 0, sizeof(o));
    object_init(&o, 1.0, 1.0, 2.0, 3.0);
    o.velocity = (Vec3){3.0, -2.0, 0.5};

    const double dt = 0.01;
    const int steps = 10000;
    sim_run(&o, 1, dt, steps);

    double t = dt * steps;
    Vec3 expected = {1.0 + 3.0 * t, 2.0 - 2.0 * t, 3.0 + 0.5 * t};
    double err = vec3_magnitude(vec3_sub(o.position, expected));
    double v_err = vec3_magnitude(vec3_sub(o.velocity, (Vec3){3.0, -2.0, 0.5}));

    report("Free flight", "position error after 100 s (m)", 1e-9, err);
    report("Free flight", "velocity change (m/s)", 1e-12, v_err);
}

/*
 * Eccentric (e = 0.5) Earth-Sun orbit for 10 periods at 2000 steps per
 * period. The period is measured from the times the relative position
 * crosses the +x axis (linear interpolation between steps) and compared to
 * Kepler's third law. Energy and angular momentum are tracked every step.
 */
static void kepler_orbit(void) {
    const double a = AU;
    const int orbits = 10;
    const int steps_per_orbit = 2000;
    const double period = two_body_period(a);
    const double dt = period / steps_per_orbit;

    PhysicsObject o[2];
    two_body_init(o, a, 0.5);

    double e0 = total_energy(o);
    double l0 = angular_momentum_z(o);
    double max_de = 0.0, max_dl = 0.0;
    double prev_y = 0.0, last_cross = 0.0;
    int crossings = 0;

    for (int step = 1; step <= orbits * steps_per_orbit; step++) {
        sim_run(o, 2, dt, 1);

        Vec3 rel = vec3_sub(o[1].position, o[0].position);
        if (prev_y < 0.0 && rel.y >= 0.0 && rel.x > 0.0) {
            last_cross = dt * (step - 1) + dt * (-prev_y) / (rel.y - prev_y);
            crossings++;
        }
        prev_y = rel.y;

        double de = fabs((total_energy(o) - e0) / e0);
        double dl = fabs((angular_momentum_z(o) - l0) / l0);
        max_de = de > max_de ? de : max_de;
        max_dl = dl > max_dl ? dl : max_dl;
    }

    double measured_period = crossings > 0 ? last_cross / crossings : 0.0;
    report("Kepler orbit (e=0.5)", "period relative error", 1e-4,
           (measured_period - period) / period);
    report("Kepler orbit (e=0.5)", "max energy drift over 10 orbits", 1e-4,
           max_de);
    report("Kepler orbit (e=0.5)", "max angular momentum drift over 10 orbits",
           1e-12, max_dl);
}

/*
 * Circular orbit for one period. The exact relative position at time t is
 * r * (cos wt, sin wt), so the error can be measured without a reference
 * run. Halving dt should cut the error by about 4x for a second order
 * integrator. Reports log2 of the last error ratio as the observed order.
 */
static double circular_error(int steps) {
    const double r = AU;
    const double period = two_body_period(r);
    const double dt = period / steps;

    PhysicsObject o[2];
    two_body_init(o, r, 0.0);
    sim_run(o, 2, dt, steps);

    Vec3 rel = vec3_sub(o[1].position, o[0].position);
    return vec3_magnitude(vec3_sub(rel, (Vec3){r, 0.0, 0.0}));
}

static void verlet_convergence(double *errors, const int *steps, int count) {
    for (int i = 0; i < count; i++)
        errors[i] = circular_error(steps[i]);

    double order = log2(errors[count - 2] / errors[count - 1]);
    report("Verlet convergence", "observed order - 2", 0.1, order - 2.0);
}

/* Unit cube centred at the origin, outward CCW faces. */
static void set_unit_cube(PhysicsObject *o) {
    static const Vec3 verts[8] = {
        {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, 0.5, -0.5},
        {-0.5, 0.5, -0.5},  {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5},
        {0.5, 0.5, 0.5},    {-0.5, 0.5, 0.5},
    };
    static const int faces[12][3] = {
        {0, 2, 1}, {0, 3, 2}, {4, 5, 6}, {4, 6, 7}, {0, 1, 5}, {0, 5, 4},
        {2, 3, 7}, {2, 7, 6}, {1, 2, 6}, {1, 6, 5}, {0, 4, 7}, {0, 7, 3},
    };
    memcpy(o->local_verts, verts, sizeof(verts));
    memcpy(o->face_indices, faces, sizeof(faces));
    o->vertex_count = 8;
    o->face_count = 12;
}

/*
 * Two overlapping unit cubes approaching each other, run for one tick
 * through the full pipeline (octree, SAT, inelastic response). sim_run uses
 * a fixed restitution of 0.5, so the relative velocity along the line of
 * centres must reverse and halve, and total momentum must not change.
 * Masses are small enough that gravity between them is negligible.
 */
static void inelastic_collision_case(void) {
    const double e = 0.5;
    PhysicsObject o[2];
    memset(o, 0, sizeof(o));
    object_init(&o[0], 2.0, 0.0, 0.0, 0.0);
    object_init(&o[1], 1.0, 0.9, 0.1, 0.0);
    o[0].velocity = (Vec3){1.0, 0.3, 0.0};
    o[1].velocity = (Vec3){-1.0, 0.0, 0.2};
    set_unit_cube(&o[0]);
    set_unit_cube(&o[1]);

    Vec3 n = vec3_normalize(vec3_sub(o[1].position, o[0].position));
    Vec3 p0 = vec3_add(vec3_scale(o[0].velocity, o[0].mass),
                       vec3_scale(o[1].velocity, o[1].mass));
    double u_rel = vec3_dot(vec3_sub(o[1].velocity, o[0].velocity), n);

    sim_run(o, 2, 1e-3, 1);

    Vec3 p1 = vec3_add(vec3_scale(o[0].velocity, o[0].mass),
                       vec3_scale(o[1].velocity, o[1].mass));
    double v_rel = vec3_dot(vec3_sub(o[1].velocity, o[0].velocity), n);

    report("Inelastic collision", "momentum change (relative)", 1e-12,
           vec3_magnitude(vec3_sub(p1, p0)) / vec3_magnitude(p0));
    report("Inelastic collision", "v_rel_after / v_rel_before + e", 1e-6,
           v_rel / u_rel + e);
}

int main(void) {
    const int steps[] = {50, 100, 200, 400};
    const int count = sizeof(steps) / sizeof(steps[0]);
    double errors[4];

    printf("# Validation Results\n\n");
    printf("Generated by `validation/validate.c`:\n\n");
    printf("```bash\nOMP_NUM_THREADS=1 ./build/validation/validate > "
           "validation/RESULTS.md\n"
           "```\n\n");
    printf("| Case | Metric | Tolerance | Measured | Result |\n");
    printf("|---|---|---|---|---|\n");

    free_flight();
    kepler_orbit();
    verlet_convergence(errors, steps, count);
    inelastic_collision_case();

    printf("\n## Verlet convergence detail\n\n");
    printf("Circular Earth-Sun orbit, error in relative position after one "
           "period.\n\n");
    printf("| Steps per orbit | Error (m) | Ratio to previous |\n");
    printf("|---|---|---|\n");
    for (int i = 0; i < count; i++) {
        if (i == 0)
            printf("| %d | %.3e | - |\n", steps[i], errors[i]);
        else
            printf("| %d | %.3e | %.2f |\n", steps[i], errors[i],
                   errors[i - 1] / errors[i]);
    }

    printf("\n## Not covered\n\n");
    printf("- Pendulum and projectile: the engine has no external or "
           "constraint forces (only N-body gravity and collisions), so "
           "neither can be set up honestly.\n");

    return failures > 0;
}
