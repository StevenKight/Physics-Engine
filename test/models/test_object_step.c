/**
 * @file test_object_step.c
 * @brief Unit tests for the Velocity Verlet step (object_update_position and
 *        object_update_velocity).
 *
 * Tests cover: object at rest with no force, uniform motion (no force),
 * first and second steps under a constant force, and force accumulator reset
 * after each step. Also checks that object_init() leaves no stale collision
 * mesh behind.
 *
 * Velocity Verlet is exact for a constant force, so the constant-force tests
 * compare against the analytic x = x0 + v0*t + a*t^2/2 and v = v0 + a*t.
 *
 * @author Steven Kight
 * @date 2026-04-10
 */

#include "object.h"
#include "test_runner.h"

#include <string.h>

/**
 * One full step the way sim_run() does it: move using the stored
 * acceleration, apply the force at the new position, then finish the
 * velocity update.
 */
static void full_step(PhysicsObject *obj, Vec3 force, double time_step) {
    object_update_position(obj, time_step);
    obj->force = force;
    object_update_velocity(obj, time_step);
}

/**
 * object_init() must clear the mesh counts. Objects are often declared on the
 * stack, and leftover memory there would otherwise look like a huge mesh to
 * collision detection.
 */
static char *test_init_clears_mesh() {
    PhysicsObject obj;
    memset(&obj, 0x5a, sizeof(obj));

    object_init(&obj, 1.0, 0.0, 0.0, 0.0);

    mu_assert("init: vertex_count != 0", obj.vertex_count == 0);
    mu_assert("init: face_count != 0", obj.face_count == 0);
    return NULL;
}

/**
 * An object at rest with no force applied must remain stationary.
 *
 * All kinematic quantities stay zero.
 */
static char *test_rest_no_force() {
    PhysicsObject obj = {
        .mass = 1.0,
        .position = {0.0, 0.0, 0.0},
        .velocity = {0.0, 0.0, 0.0},
        .acceleration = {0.0, 0.0, 0.0},
        .force = {0.0, 0.0, 0.0},
    };

    full_step(&obj, (Vec3){0.0, 0.0, 0.0}, 1.0);

    mu_assert_double_eq("rest: pos.x != 0", obj.position.x, 0.0, 1e-15);
    mu_assert_double_eq("rest: pos.y != 0", obj.position.y, 0.0, 1e-15);
    mu_assert_double_eq("rest: pos.z != 0", obj.position.z, 0.0, 1e-15);
    mu_assert_double_eq("rest: vel.x != 0", obj.velocity.x, 0.0, 1e-15);
    mu_assert_double_eq("rest: vel.y != 0", obj.velocity.y, 0.0, 1e-15);
    mu_assert_double_eq("rest: vel.z != 0", obj.velocity.z, 0.0, 1e-15);
    return NULL;
}

/**
 * An object moving with constant velocity and no force must travel in a
 * straight line: position advances by v * dt, velocity is unchanged.
 *
 * Initial: pos = (1, 2, 3), vel = (4, 5, 6), acc = 0, force = 0, dt = 0.5
 * Expected: pos = (3, 4.5, 6), vel = (4, 5, 6)
 */
static char *test_uniform_motion() {
    PhysicsObject obj = {
        .mass = 1.0,
        .position = {1.0, 2.0, 3.0},
        .velocity = {4.0, 5.0, 6.0},
        .acceleration = {0.0, 0.0, 0.0},
        .force = {0.0, 0.0, 0.0},
    };

    full_step(&obj, (Vec3){0.0, 0.0, 0.0}, 0.5);

    mu_assert_double_eq("uniform: pos.x != 3", obj.position.x, 3.0, 1e-15);
    mu_assert_double_eq("uniform: pos.y != 4.5", obj.position.y, 4.5, 1e-15);
    mu_assert_double_eq("uniform: pos.z != 6", obj.position.z, 6.0, 1e-15);
    mu_assert_double_eq("uniform: vel.x != 4", obj.velocity.x, 4.0, 1e-15);
    mu_assert_double_eq("uniform: vel.y != 5", obj.velocity.y, 5.0, 1e-15);
    mu_assert_double_eq("uniform: vel.z != 6", obj.velocity.z, 6.0, 1e-15);
    return NULL;
}

/**
 * First step from rest under a constant x-axis force.
 *
 * The starting acceleration is already set (sim_run computes it from the
 * initial positions before the first tick).
 *
 *   v_half = 0 + 2/2 * 1 = 1
 *   x      = 0 + 1 * 1   = 1     (exact: a*t^2/2 = 1)
 *   v      = 1 + 2/2 * 1 = 2     (exact: a*t     = 2)
 *
 * Initial: mass=2, pos=0, vel=0, acc=(2,0,0), force=(4,0,0), dt=1
 * Expected: pos=(1,0,0), vel=(2,0,0), acc=(2,0,0)
 */
static char *test_constant_force_first_step() {
    PhysicsObject obj = {
        .mass = 2.0,
        .position = {0.0, 0.0, 0.0},
        .velocity = {0.0, 0.0, 0.0},
        .acceleration = {2.0, 0.0, 0.0},
        .force = {0.0, 0.0, 0.0},
    };

    full_step(&obj, (Vec3){4.0, 0.0, 0.0}, 1.0);

    mu_assert_double_eq("1st step: pos.x != 1", obj.position.x, 1.0, 1e-15);
    mu_assert_double_eq("1st step: pos.y != 0", obj.position.y, 0.0, 1e-15);
    mu_assert_double_eq("1st step: pos.z != 0", obj.position.z, 0.0, 1e-15);
    mu_assert_double_eq("1st step: vel.x != 2", obj.velocity.x, 2.0, 1e-15);
    mu_assert_double_eq("1st step: vel.y != 0", obj.velocity.y, 0.0, 1e-15);
    mu_assert_double_eq("1st step: vel.z != 0", obj.velocity.z, 0.0, 1e-15);
    mu_assert_double_eq("1st step: acc.x != 2", obj.acceleration.x, 2.0, 1e-15);
    return NULL;
}

/**
 * Second step, continuing from the state after the first.
 *
 *   v_half = 2 + 2/2 * 1 = 3
 *   x      = 1 + 3 * 1   = 4     (exact at t=2: a*t^2/2 = 4)
 *   v      = 3 + 2/2 * 1 = 4     (exact at t=2: a*t     = 4)
 *
 * Initial: mass=2, pos=(1,0,0), vel=(2,0,0), acc=(2,0,0), force=(4,0,0), dt=1
 * Expected: pos=(4,0,0), vel=(4,0,0), acc=(2,0,0)
 */
static char *test_constant_force_second_step() {
    PhysicsObject obj = {
        .mass = 2.0,
        .position = {1.0, 0.0, 0.0},
        .velocity = {2.0, 0.0, 0.0},
        .acceleration = {2.0, 0.0, 0.0},
        .force = {0.0, 0.0, 0.0},
    };

    full_step(&obj, (Vec3){4.0, 0.0, 0.0}, 1.0);

    mu_assert_double_eq("2nd step: pos.x != 4", obj.position.x, 4.0, 1e-15);
    mu_assert_double_eq("2nd step: pos.y != 0", obj.position.y, 0.0, 1e-15);
    mu_assert_double_eq("2nd step: pos.z != 0", obj.position.z, 0.0, 1e-15);
    mu_assert_double_eq("2nd step: vel.x != 4", obj.velocity.x, 4.0, 1e-15);
    mu_assert_double_eq("2nd step: vel.y != 0", obj.velocity.y, 0.0, 1e-15);
    mu_assert_double_eq("2nd step: vel.z != 0", obj.velocity.z, 0.0, 1e-15);
    return NULL;
}

/**
 * After every object_update_velocity(), the force accumulator must be zeroed
 * so the next round of force additions starts from a clean slate.
 */
static char *test_force_reset() {
    PhysicsObject obj = {
        .mass = 1.0,
        .position = {0.0, 0.0, 0.0},
        .velocity = {0.0, 0.0, 0.0},
        .acceleration = {0.0, 0.0, 0.0},
        .force = {0.0, 0.0, 0.0},
    };

    full_step(&obj, (Vec3){3.0, 5.0, 7.0}, 1.0);

    mu_assert_double_eq("force reset: force.x != 0", obj.force.x, 0.0, 1e-15);
    mu_assert_double_eq("force reset: force.y != 0", obj.force.y, 0.0, 1e-15);
    mu_assert_double_eq("force reset: force.z != 0", obj.force.z, 0.0, 1e-15);
    return NULL;
}

static const TestCase tests[] = {
    {"init_clears_mesh", test_init_clears_mesh},
    {"rest_no_force", test_rest_no_force},
    {"uniform_motion", test_uniform_motion},
    {"constant_force_first_step", test_constant_force_first_step},
    {"constant_force_second_step", test_constant_force_second_step},
    {"force_reset", test_force_reset},
};

int main(void) {
    int failed = run_suite("Object Step (Velocity Verlet)", tests,
                           sizeof(tests) / sizeof(tests[0]));
    return finish_suite(failed);
}
