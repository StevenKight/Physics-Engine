/**
 * @file test_aabb.c
 * @brief Unit tests for AABB helpers.
 *
 * @author Steven Kight
 */

#include "collision/aabb.h"
#include "test_runner.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/* aabb_from_object                                                       */
/* ------------------------------------------------------------------ */

static char *test_aabb_point_object() {
    /* vertex_count == 0 → AABB is a single point at position */
    PhysicsObject obj;
    memset(&obj, 0, sizeof(obj));
    obj.position = (Vec3){ 3.0, -1.0, 2.0 };
    obj.vertex_count = 0;

    AABB box = aabb_from_object(&obj);

    mu_assert_double_eq("min.x", box.min.x, 3.0,  1e-15);
    mu_assert_double_eq("min.y", box.min.y, -1.0, 1e-15);
    mu_assert_double_eq("min.z", box.min.z, 2.0,  1e-15);
    mu_assert_double_eq("max.x", box.max.x, 3.0,  1e-15);
    mu_assert_double_eq("max.y", box.max.y, -1.0, 1e-15);
    mu_assert_double_eq("max.z", box.max.z, 2.0,  1e-15);
    return NULL;
}

static char *test_aabb_unit_cube_at_origin() {
    /* Unit cube centred at origin: verts at ±0.5 on each axis */
    PhysicsObject obj;
    memset(&obj, 0, sizeof(obj));
    obj.position = (Vec3){ 0.0, 0.0, 0.0 };
    obj.vertex_count = 8;
    obj.local_verts[0] = (Vec3){ -0.5, -0.5, -0.5 };
    obj.local_verts[1] = (Vec3){  0.5, -0.5, -0.5 };
    obj.local_verts[2] = (Vec3){  0.5,  0.5, -0.5 };
    obj.local_verts[3] = (Vec3){ -0.5,  0.5, -0.5 };
    obj.local_verts[4] = (Vec3){ -0.5, -0.5,  0.5 };
    obj.local_verts[5] = (Vec3){  0.5, -0.5,  0.5 };
    obj.local_verts[6] = (Vec3){  0.5,  0.5,  0.5 };
    obj.local_verts[7] = (Vec3){ -0.5,  0.5,  0.5 };

    AABB box = aabb_from_object(&obj);

    mu_assert_double_eq("min.x", box.min.x, -0.5, 1e-15);
    mu_assert_double_eq("min.y", box.min.y, -0.5, 1e-15);
    mu_assert_double_eq("min.z", box.min.z, -0.5, 1e-15);
    mu_assert_double_eq("max.x", box.max.x,  0.5, 1e-15);
    mu_assert_double_eq("max.y", box.max.y,  0.5, 1e-15);
    mu_assert_double_eq("max.z", box.max.z,  0.5, 1e-15);
    return NULL;
}

static char *test_aabb_offset_position() {
    /* Same unit cube translated to (2, 3, 4) */
    PhysicsObject obj;
    memset(&obj, 0, sizeof(obj));
    obj.position = (Vec3){ 2.0, 3.0, 4.0 };
    obj.vertex_count = 8;
    obj.local_verts[0] = (Vec3){ -0.5, -0.5, -0.5 };
    obj.local_verts[1] = (Vec3){  0.5, -0.5, -0.5 };
    obj.local_verts[2] = (Vec3){  0.5,  0.5, -0.5 };
    obj.local_verts[3] = (Vec3){ -0.5,  0.5, -0.5 };
    obj.local_verts[4] = (Vec3){ -0.5, -0.5,  0.5 };
    obj.local_verts[5] = (Vec3){  0.5, -0.5,  0.5 };
    obj.local_verts[6] = (Vec3){  0.5,  0.5,  0.5 };
    obj.local_verts[7] = (Vec3){ -0.5,  0.5,  0.5 };

    AABB box = aabb_from_object(&obj);

    mu_assert_double_eq("min.x", box.min.x, 1.5, 1e-15);
    mu_assert_double_eq("min.y", box.min.y, 2.5, 1e-15);
    mu_assert_double_eq("min.z", box.min.z, 3.5, 1e-15);
    mu_assert_double_eq("max.x", box.max.x, 2.5, 1e-15);
    mu_assert_double_eq("max.y", box.max.y, 3.5, 1e-15);
    mu_assert_double_eq("max.z", box.max.z, 4.5, 1e-15);
    return NULL;
}

/* ------------------------------------------------------------------ */
/* aabb_overlaps                                                          */
/* ------------------------------------------------------------------ */

static char *test_overlap_separated_x() {
    AABB a = { .min = {0,0,0}, .max = {1,1,1} };
    AABB b = { .min = {2,0,0}, .max = {3,1,1} };  /* gap on x */
    mu_assert("separated on x should not overlap", !aabb_overlaps(a, b));
    return NULL;
}

static char *test_overlap_touching_face() {
    AABB a = { .min = {0,0,0}, .max = {1,1,1} };
    AABB b = { .min = {1,0,0}, .max = {2,1,1} };  /* touching on x face */
    mu_assert("touching faces should overlap", aabb_overlaps(a, b));
    return NULL;
}

static char *test_overlap_fully_overlapping() {
    AABB a = { .min = {0,0,0}, .max = {2,2,2} };
    AABB b = { .min = {0.5,0.5,0.5}, .max = {1.5,1.5,1.5} };  /* b inside a */
    mu_assert("nested boxes should overlap", aabb_overlaps(a, b));
    return NULL;
}

/* ------------------------------------------------------------------ */
/* aabb_split_octants                                                    */
/* ------------------------------------------------------------------ */

static char *test_split_octants_count_and_bounds() {
    AABB parent = { .min = {-1,-1,-1}, .max = {1,1,1} };
    AABB out[8];
    aabb_split_octants(parent, out);

    /* Centre should be at (0,0,0). Each child is a unit cube. */
    for (int i = 0; i < 8; i++) {
        double size_x = out[i].max.x - out[i].min.x;
        double size_y = out[i].max.y - out[i].min.y;
        double size_z = out[i].max.z - out[i].min.z;
        mu_assert_double_eq("child size x", size_x, 1.0, 1e-15);
        mu_assert_double_eq("child size y", size_y, 1.0, 1e-15);
        mu_assert_double_eq("child size z", size_z, 1.0, 1e-15);
    }

    /* Child 7 (bits 1|2|4 set) is the (+x,+y,+z) octant */
    mu_assert_double_eq("child7 min.x", out[7].min.x, 0.0, 1e-15);
    mu_assert_double_eq("child7 max.x", out[7].max.x, 1.0, 1e-15);
    mu_assert_double_eq("child7 min.y", out[7].min.y, 0.0, 1e-15);
    mu_assert_double_eq("child7 max.y", out[7].max.y, 1.0, 1e-15);
    mu_assert_double_eq("child7 min.z", out[7].min.z, 0.0, 1e-15);
    mu_assert_double_eq("child7 max.z", out[7].max.z, 1.0, 1e-15);

    /* Child 0 (no bits set) is the (-x,-y,-z) octant */
    mu_assert_double_eq("child0 min.x", out[0].min.x, -1.0, 1e-15);
    mu_assert_double_eq("child0 max.x", out[0].max.x,  0.0, 1e-15);
    return NULL;
}

static const TestCase tests[] = {
    {"aabb_point_object",       test_aabb_point_object},
    {"aabb_unit_cube_at_origin",test_aabb_unit_cube_at_origin},
    {"aabb_offset_position",    test_aabb_offset_position},
    {"overlap_separated_x",     test_overlap_separated_x},
    {"overlap_touching_face",   test_overlap_touching_face},
    {"overlap_fully_overlapping",test_overlap_fully_overlapping},
    {"split_octants",           test_split_octants_count_and_bounds},
};

int main(void) {
    int failed = run_suite("AABB", tests, sizeof(tests) / sizeof(tests[0]));
    return finish_suite(failed);
}
