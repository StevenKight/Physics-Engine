/**
 * @file test_inelastic_collision.c
 * @brief Unit tests for inelastic_collision().
 *
 * All tests use objects separated along the x-axis so the collision normal
 * is (1,0,0) and expected values can be verified by hand.
 *
 * The function applies the velocity change directly (impulse-based) rather
 * than via obj->force, because Velocity Verlet uses a_old for the position
 * update — adding a force would not prevent objects from passing through
 * each other in the same tick.
 *
 * Formula under test (from docs/Math-Newtonian-Collisions.md):
 *   va = (C_R·mb·(ub−ua) + ma·ua + mb·ub) / (ma+mb)
 *   vb = (C_R·ma·(ua−ub) + ma·ua + mb·ub) / (ma+mb)
 *
 * @author Steven Kight
 */

#include "forces/collision.h"
#include "test_runner.h"
#include <string.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/* Helpers                                                               */
/* ------------------------------------------------------------------ */

static PhysicsObject make_obj(double mass,
                               double px, double py, double pz,
                               double vx, double vy, double vz) {
    PhysicsObject obj;
    memset(&obj, 0, sizeof(obj));
    obj.mass     = mass;
    obj.position = (Vec3){ px, py, pz };
    obj.velocity = (Vec3){ vx, vy, vz };
    obj.force    = (Vec3){ 0.0, 0.0, 0.0 };
    return obj;
}

/* ------------------------------------------------------------------ */
/* Tests                                                                 */
/* ------------------------------------------------------------------ */

/*
 * Equal masses, head-on, elastic (C_R=1): velocities exchange along the normal.
 *   ua=+1, ub=-1  →  va=-1, vb=+1
 */
static char *test_elastic_equal_mass() {
    PhysicsObject a = make_obj(1.0,  0.0,0.0,0.0,  1.0,0.0,0.0);
    PhysicsObject b = make_obj(1.0,  1.0,0.0,0.0, -1.0,0.0,0.0);

    inelastic_collision(&a, &b, 1.0);

    printf("    v_a=(%.4f,%.4f,%.4f)  v_b=(%.4f,%.4f,%.4f)\n",
           a.velocity.x, a.velocity.y, a.velocity.z,
           b.velocity.x, b.velocity.y, b.velocity.z);

    mu_assert_double_eq("elastic: v_a.x != -1", a.velocity.x, -1.0, 1e-10);
    mu_assert_double_eq("elastic: v_a.y != 0",  a.velocity.y,  0.0, 1e-10);
    mu_assert_double_eq("elastic: v_a.z != 0",  a.velocity.z,  0.0, 1e-10);
    mu_assert_double_eq("elastic: v_b.x != +1", b.velocity.x,  1.0, 1e-10);
    mu_assert_double_eq("elastic: v_b.y != 0",  b.velocity.y,  0.0, 1e-10);
    mu_assert_double_eq("elastic: v_b.z != 0",  b.velocity.z,  0.0, 1e-10);
    return NULL;
}

/*
 * Equal masses, head-on, perfectly inelastic (C_R=0): both come to rest.
 *   ua=+1, ub=-1  →  va=0, vb=0
 */
static char *test_perfectly_inelastic_equal_mass() {
    PhysicsObject a = make_obj(1.0,  0.0,0.0,0.0,  1.0,0.0,0.0);
    PhysicsObject b = make_obj(1.0,  1.0,0.0,0.0, -1.0,0.0,0.0);

    inelastic_collision(&a, &b, 0.0);

    printf("    v_a=(%.4f,%.4f,%.4f)  v_b=(%.4f,%.4f,%.4f)\n",
           a.velocity.x, a.velocity.y, a.velocity.z,
           b.velocity.x, b.velocity.y, b.velocity.z);

    mu_assert_double_eq("perf-inelastic: v_a.x != 0", a.velocity.x, 0.0, 1e-10);
    mu_assert_double_eq("perf-inelastic: v_b.x != 0", b.velocity.x, 0.0, 1e-10);
    mu_assert_double_eq("perf-inelastic: v_a.y != 0", a.velocity.y, 0.0, 1e-10);
    mu_assert_double_eq("perf-inelastic: v_b.y != 0", b.velocity.y, 0.0, 1e-10);
    return NULL;
}

/*
 * Unequal masses, C_R=0.5, b initially at rest.
 *   ma=2, mb=1,  ua=2, ub=0
 *   va = (0.5·1·(0−2) + 2·2 + 1·0) / 3 = (−1+4)/3 = 1
 *   vb = (0.5·2·(2−0) + 2·2 + 1·0) / 3 = (2+4)/3  = 2
 */
static char *test_unequal_masses() {
    PhysicsObject a = make_obj(2.0,  0.0,0.0,0.0,  2.0,0.0,0.0);
    PhysicsObject b = make_obj(1.0,  1.0,0.0,0.0,  0.0,0.0,0.0);

    inelastic_collision(&a, &b, 0.5);

    printf("    v_a=(%.4f,%.4f,%.4f)  v_b=(%.4f,%.4f,%.4f)\n",
           a.velocity.x, a.velocity.y, a.velocity.z,
           b.velocity.x, b.velocity.y, b.velocity.z);

    mu_assert_double_eq("unequal: v_a.x != 1", a.velocity.x, 1.0, 1e-10);
    mu_assert_double_eq("unequal: v_b.x != 2", b.velocity.x, 2.0, 1e-10);
    mu_assert_double_eq("unequal: v_a.y != 0", a.velocity.y, 0.0, 1e-10);
    mu_assert_double_eq("unequal: v_b.y != 0", b.velocity.y, 0.0, 1e-10);
    return NULL;
}

/*
 * Newton's third law / momentum conservation:
 *   ma·va + mb·vb must equal ma·ua + mb·ub (total momentum unchanged).
 */
static char *test_momentum_conserved() {
    double ma = 3.0, mb = 5.0;
    PhysicsObject a = make_obj(ma,  0.0,0.0,0.0,  4.0,0.0,0.0);
    PhysicsObject b = make_obj(mb,  1.0,0.0,0.0, -2.0,0.0,0.0);

    double p_before = ma * 4.0 + mb * (-2.0);

    inelastic_collision(&a, &b, 0.7);

    double p_after = ma * a.velocity.x + mb * b.velocity.x;

    printf("    p_before=%.6e  p_after=%.6e\n", p_before, p_after);

    mu_assert_double_eq("momentum: px not conserved", p_before, p_after, 1e-10);
    mu_assert_double_eq("momentum: py not conserved", 0.0, ma*a.velocity.y + mb*b.velocity.y, 1e-10);
    mu_assert_double_eq("momentum: pz not conserved", 0.0, ma*a.velocity.z + mb*b.velocity.z, 1e-10);
    return NULL;
}

/*
 * Already separating: objects moving away from each other along the normal.
 * Velocity must not change.
 */
static char *test_already_separating() {
    PhysicsObject a = make_obj(1.0,  0.0,0.0,0.0, -1.0,0.0,0.0);
    PhysicsObject b = make_obj(1.0,  1.0,0.0,0.0,  1.0,0.0,0.0);

    inelastic_collision(&a, &b, 0.5);

    mu_assert_double_eq("separating: v_a.x changed", a.velocity.x, -1.0, 1e-10);
    mu_assert_double_eq("separating: v_b.x changed", b.velocity.x,  1.0, 1e-10);
    return NULL;
}

/*
 * Coincident centres: distance < 1e-10, no well-defined normal.
 * Velocity must not change.
 */
static char *test_coincident_centres() {
    PhysicsObject a = make_obj(1.0,  0.0,0.0,0.0,  1.0,0.0,0.0);
    PhysicsObject b = make_obj(1.0,  0.0,0.0,0.0, -1.0,0.0,0.0);

    inelastic_collision(&a, &b, 0.5);

    mu_assert_double_eq("coincident: v_a.x changed", a.velocity.x,  1.0, 1e-10);
    mu_assert_double_eq("coincident: v_b.x changed", b.velocity.x, -1.0, 1e-10);
    return NULL;
}

/*
 * Tangential velocity is untouched: objects have non-zero y velocity but
 * only collide along x. The y component of velocity must be unchanged.
 */
static char *test_tangential_velocity_unchanged() {
    PhysicsObject a = make_obj(1.0,  0.0,0.0,0.0,  1.0,3.0,0.0);
    PhysicsObject b = make_obj(1.0,  1.0,0.0,0.0, -1.0,3.0,0.0);

    inelastic_collision(&a, &b, 1.0);

    printf("    v_a=(%.4f,%.4f,%.4f)  v_b=(%.4f,%.4f,%.4f)\n",
           a.velocity.x, a.velocity.y, a.velocity.z,
           b.velocity.x, b.velocity.y, b.velocity.z);

    mu_assert_double_eq("tangential: v_a.y changed", a.velocity.y, 3.0, 1e-10);
    mu_assert_double_eq("tangential: v_a.z changed", a.velocity.z, 0.0, 1e-10);
    mu_assert_double_eq("tangential: v_b.y changed", b.velocity.y, 3.0, 1e-10);
    mu_assert_double_eq("tangential: v_b.z changed", b.velocity.z, 0.0, 1e-10);
    return NULL;
}

/*
 * obj->force must not be touched: collision response is velocity-only.
 * Pre-existing gravity force on each object must be preserved unchanged.
 */
static char *test_force_untouched() {
    PhysicsObject a = make_obj(1.0,  0.0,0.0,0.0,  1.0,0.0,0.0);
    PhysicsObject b = make_obj(1.0,  1.0,0.0,0.0, -1.0,0.0,0.0);
    a.force = (Vec3){ 5.0, 2.0, 0.0 };
    b.force = (Vec3){ 3.0, 1.0, 0.0 };

    inelastic_collision(&a, &b, 0.5);

    mu_assert_double_eq("force: a.force.x changed", a.force.x, 5.0, 1e-10);
    mu_assert_double_eq("force: a.force.y changed", a.force.y, 2.0, 1e-10);
    mu_assert_double_eq("force: b.force.x changed", b.force.x, 3.0, 1e-10);
    mu_assert_double_eq("force: b.force.y changed", b.force.y, 1.0, 1e-10);
    return NULL;
}

/* ------------------------------------------------------------------ */
/* Suite                                                                 */
/* ------------------------------------------------------------------ */

static const TestCase tests[] = {
    {"elastic_equal_mass",            test_elastic_equal_mass},
    {"perfectly_inelastic_equal_mass",test_perfectly_inelastic_equal_mass},
    {"unequal_masses",                test_unequal_masses},
    {"momentum_conserved",            test_momentum_conserved},
    {"already_separating",            test_already_separating},
    {"coincident_centres",            test_coincident_centres},
    {"tangential_velocity_unchanged", test_tangential_velocity_unchanged},
    {"force_untouched",               test_force_untouched},
};

int main(void) {
    int failed = run_suite("Inelastic Collision", tests,
                           sizeof(tests) / sizeof(tests[0]));
    return finish_suite(failed);
}
