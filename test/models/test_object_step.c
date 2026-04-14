/**
 * @file test_object_step.c
 * @brief Unit tests for the step() function (Velocity Verlet Integration).
 *
 * Tests cover: object at rest with no force, uniform motion (no force),
 * first step from rest under constant force, subsequent step with prior
 * acceleration, and force accumulator reset after each step.
 *
 * @author Steven Kight
 * @date 2026-04-10
 */

#include "object.h"
#include "test_runner.h"

/**
 * An object at rest with no force applied must remain stationary.
 *
 * All kinematic quantities stay zero.
 */
static char *test_rest_no_force() {
    PhysicsObject obj = {
        .mass         = 1.0,
        .position     = {0.0, 0.0, 0.0},
        .velocity     = {0.0, 0.0, 0.0},
        .acceleration = {0.0, 0.0, 0.0},
        .force        = {0.0, 0.0, 0.0},
    };

    object_step(&obj, 1.0);

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
        .mass         = 1.0,
        .position     = {1.0, 2.0, 3.0},
        .velocity     = {4.0, 5.0, 6.0},
        .acceleration = {0.0, 0.0, 0.0},
        .force        = {0.0, 0.0, 0.0},
    };

    object_step(&obj, 0.5);

    mu_assert_double_eq("uniform: pos.x != 3",   obj.position.x, 3.0, 1e-15);
    mu_assert_double_eq("uniform: pos.y != 4.5", obj.position.y, 4.5, 1e-15);
    mu_assert_double_eq("uniform: pos.z != 6",   obj.position.z, 6.0, 1e-15);
    mu_assert_double_eq("uniform: vel.x != 4",   obj.velocity.x, 4.0, 1e-15);
    mu_assert_double_eq("uniform: vel.y != 5",   obj.velocity.y, 5.0, 1e-15);
    mu_assert_double_eq("uniform: vel.z != 6",   obj.velocity.z, 6.0, 1e-15);
    return NULL;
}

/**
 * First step from rest under a constant x-axis force.
 *
 * With zero initial velocity and acceleration, the position formula gives
 * x_{t+dt} = 0 (both v_t and a_t are zero). Velocity picks up half the new
 * acceleration because the Verlet average is (a_t + a_{t+dt}) / 2 = (0 + 2) / 2.
 *
 * Initial: mass=2, pos=0, vel=0, acc=0, force=(4,0,0), dt=1
 * Expected: pos=(0,0,0), vel=(1,0,0), acc=(2,0,0)
 */
static char *test_constant_force_first_step() {
    PhysicsObject obj = {
        .mass         = 2.0,
        .position     = {0.0, 0.0, 0.0},
        .velocity     = {0.0, 0.0, 0.0},
        .acceleration = {0.0, 0.0, 0.0},
        .force        = {4.0, 0.0, 0.0},
    };

    object_step(&obj, 1.0);

    mu_assert_double_eq("1st step: pos.x != 0", obj.position.x, 0.0, 1e-15);
    mu_assert_double_eq("1st step: pos.y != 0", obj.position.y, 0.0, 1e-15);
    mu_assert_double_eq("1st step: pos.z != 0", obj.position.z, 0.0, 1e-15);
    mu_assert_double_eq("1st step: vel.x != 1", obj.velocity.x, 1.0, 1e-15);
    mu_assert_double_eq("1st step: vel.y != 0", obj.velocity.y, 0.0, 1e-15);
    mu_assert_double_eq("1st step: vel.z != 0", obj.velocity.z, 0.0, 1e-15);
    mu_assert_double_eq("1st step: acc.x != 2", obj.acceleration.x, 2.0, 1e-15);
    return NULL;
}

/**
 * Subsequent step with prior acceleration already set (simulates a second step).
 *
 * This exercises the Verlet average: (a_t + a_{t+dt}) / 2 = (2 + 2) / 2 = 2.
 *
 * Initial: mass=2, pos=(0,0,0), vel=(1,0,0), acc=(2,0,0), force=(4,0,0), dt=1
 *   next_a   = 4/2 = 2
 *   next_vel = 1 + (2+2)/2 * 1 = 3
 *   next_pos = 0 + 1*1 + 2/2*1^2 = 2
 * Expected: pos=(2,0,0), vel=(3,0,0), acc=(2,0,0)
 */
static char *test_constant_force_second_step() {
    PhysicsObject obj = {
        .mass         = 2.0,
        .position     = {0.0, 0.0, 0.0},
        .velocity     = {1.0, 0.0, 0.0},
        .acceleration = {2.0, 0.0, 0.0},
        .force        = {4.0, 0.0, 0.0},
    };

    object_step(&obj, 1.0);

    mu_assert_double_eq("2nd step: pos.x != 2", obj.position.x, 2.0, 1e-15);
    mu_assert_double_eq("2nd step: pos.y != 0", obj.position.y, 0.0, 1e-15);
    mu_assert_double_eq("2nd step: pos.z != 0", obj.position.z, 0.0, 1e-15);
    mu_assert_double_eq("2nd step: vel.x != 3", obj.velocity.x, 3.0, 1e-15);
    mu_assert_double_eq("2nd step: vel.y != 0", obj.velocity.y, 0.0, 1e-15);
    mu_assert_double_eq("2nd step: vel.z != 0", obj.velocity.z, 0.0, 1e-15);
    return NULL;
}

/**
 * After every step(), the force accumulator must be zeroed so the next
 * round of force additions starts from a clean slate.
 */
static char *test_force_reset() {
    PhysicsObject obj = {
        .mass         = 1.0,
        .position     = {0.0, 0.0, 0.0},
        .velocity     = {0.0, 0.0, 0.0},
        .acceleration = {0.0, 0.0, 0.0},
        .force        = {3.0, 5.0, 7.0},
    };

    object_step(&obj, 1.0);

    mu_assert_double_eq("force reset: force.x != 0", obj.force.x, 0.0, 1e-15);
    mu_assert_double_eq("force reset: force.y != 0", obj.force.y, 0.0, 1e-15);
    mu_assert_double_eq("force reset: force.z != 0", obj.force.z, 0.0, 1e-15);
    return NULL;
}

static const TestCase tests[] = {
    {"rest_no_force",             test_rest_no_force},
    {"uniform_motion",            test_uniform_motion},
    {"constant_force_first_step", test_constant_force_first_step},
    {"constant_force_second_step",test_constant_force_second_step},
    {"force_reset",               test_force_reset},
};

int main(void) {
    int failed = run_suite("Object Step (Velocity Verlet)", tests,
                           sizeof(tests) / sizeof(tests[0]));
    return finish_suite(failed);
}
