/**
 * @file test_collision.c
 * @brief Unit tests for the full collision detection pipeline.
 *
 * All tests use unit cubes (8 vertices, 12 triangular faces) because their
 * geometry is trivially verifiable by hand.
 *
 * @author Steven Kight
 */

#include "collision/collision.h"
#include "collision/sat.h"
#include "test_runner.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/* Test fixtures                                                          */
/* ------------------------------------------------------------------ */

/*
 * Unit cube centred at the origin in local space.
 * 8 vertices, 12 triangular faces (2 per axis-aligned square face), CCW winding.
 */
static void make_unit_cube(PhysicsObject *obj, double mass,
                             double x, double y, double z) {
    memset(obj, 0, sizeof(*obj));
    obj->mass     = mass;
    obj->position = (Vec3){ x, y, z };

    obj->vertex_count = 8;
    obj->local_verts[0] = (Vec3){ -0.5, -0.5, -0.5 };
    obj->local_verts[1] = (Vec3){  0.5, -0.5, -0.5 };
    obj->local_verts[2] = (Vec3){  0.5,  0.5, -0.5 };
    obj->local_verts[3] = (Vec3){ -0.5,  0.5, -0.5 };
    obj->local_verts[4] = (Vec3){ -0.5, -0.5,  0.5 };
    obj->local_verts[5] = (Vec3){  0.5, -0.5,  0.5 };
    obj->local_verts[6] = (Vec3){  0.5,  0.5,  0.5 };
    obj->local_verts[7] = (Vec3){ -0.5,  0.5,  0.5 };

    obj->face_count = 12;
    /* -z face */  obj->face_indices[0][0]=0; obj->face_indices[0][1]=1; obj->face_indices[0][2]=2;
                   obj->face_indices[1][0]=0; obj->face_indices[1][1]=2; obj->face_indices[1][2]=3;
    /* +z face */  obj->face_indices[2][0]=4; obj->face_indices[2][1]=6; obj->face_indices[2][2]=5;
                   obj->face_indices[3][0]=4; obj->face_indices[3][1]=7; obj->face_indices[3][2]=6;
    /* -x face */  obj->face_indices[4][0]=0; obj->face_indices[4][1]=3; obj->face_indices[4][2]=7;
                   obj->face_indices[5][0]=0; obj->face_indices[5][1]=7; obj->face_indices[5][2]=4;
    /* +x face */  obj->face_indices[6][0]=1; obj->face_indices[6][1]=5; obj->face_indices[6][2]=6;
                   obj->face_indices[7][0]=1; obj->face_indices[7][1]=6; obj->face_indices[7][2]=2;
    /* -y face */  obj->face_indices[8][0]=0; obj->face_indices[8][1]=4; obj->face_indices[8][2]=5;
                   obj->face_indices[9][0]=0; obj->face_indices[9][1]=5; obj->face_indices[9][2]=1;
    /* +y face */  obj->face_indices[10][0]=3; obj->face_indices[10][1]=2; obj->face_indices[10][2]=6;
                   obj->face_indices[11][0]=3; obj->face_indices[11][1]=6; obj->face_indices[11][2]=7;
}

/* ------------------------------------------------------------------ */
/* sat_test_one                                                           */
/* ------------------------------------------------------------------ */

static char *test_sat_separated() {
    PhysicsObject a, b;
    make_unit_cube(&a, 1.0,  0.0, 0.0, 0.0);
    make_unit_cube(&b, 1.0, 10.0, 0.0, 0.0);  /* 10 m away — clear gap */
    mu_assert("separated cubes must not intersect", !sat_test_one(&a, &b));
    return NULL;
}

static char *test_sat_overlapping() {
    PhysicsObject a, b;
    make_unit_cube(&a, 1.0, 0.0, 0.0, 0.0);
    make_unit_cube(&b, 1.0, 0.4, 0.0, 0.0);  /* centres 0.4 m apart → overlap */
    mu_assert("overlapping cubes must intersect", sat_test_one(&a, &b));
    return NULL;
}

static char *test_sat_touching_face() {
    /* Faces exactly touching on x axis — convention: touching counts as overlap */
    PhysicsObject a, b;
    make_unit_cube(&a, 1.0, 0.0, 0.0, 0.0);
    make_unit_cube(&b, 1.0, 1.0, 0.0, 0.0);  /* gap is exactly 0 */
    /* Touching = intervals_overlap returns 1 (max_a >= min_b) */
    mu_assert("touching face-to-face must be counted as overlap",
              sat_test_one(&a, &b));
    return NULL;
}

/* ------------------------------------------------------------------ */
/* collision_detect                                                       */
/* ------------------------------------------------------------------ */

static char *test_detect_empty() {
    CollisionPair pairs[16];
    int n = collision_detect(NULL, 0, pairs, 16);
    mu_assert("zero objects → zero pairs", n == 0);
    return NULL;
}

static char *test_detect_single() {
    PhysicsObject obj;
    make_unit_cube(&obj, 1.0, 0.0, 0.0, 0.0);
    CollisionPair pairs[16];
    int n = collision_detect(&obj, 1, pairs, 16);
    mu_assert("single object → zero pairs", n == 0);
    return NULL;
}

static char *test_detect_two_separated() {
    PhysicsObject objects[2];
    make_unit_cube(&objects[0], 1.0,  0.0, 0.0, 0.0);
    make_unit_cube(&objects[1], 1.0, 10.0, 0.0, 0.0);
    CollisionPair pairs[16];
    int n = collision_detect(objects, 2, pairs, 16);
    mu_assert("separated cubes → 0 pairs", n == 0);
    return NULL;
}

static char *test_detect_two_overlapping() {
    PhysicsObject objects[2];
    make_unit_cube(&objects[0], 1.0, 0.0, 0.0, 0.0);
    make_unit_cube(&objects[1], 1.0, 0.4, 0.0, 0.0);
    CollisionPair pairs[16];
    int n = collision_detect(objects, 2, pairs, 16);
    mu_assert("overlapping cubes → 1 pair", n == 1);
    mu_assert("pair index_a == 0", pairs[0].index_a == 0);
    mu_assert("pair index_b == 1", pairs[0].index_b == 1);
    return NULL;
}

static char *test_detect_three_one_overlap() {
    PhysicsObject objects[3];
    make_unit_cube(&objects[0], 1.0,  0.0, 0.0, 0.0);
    make_unit_cube(&objects[1], 1.0,  0.4, 0.0, 0.0);  /* overlaps [0] */
    make_unit_cube(&objects[2], 1.0, 50.0, 0.0, 0.0);  /* far away */
    CollisionPair pairs[16];
    int n = collision_detect(objects, 3, pairs, 16);
    mu_assert("three objects, one overlap → 1 pair", n == 1);
    mu_assert("pair is (0,1)", pairs[0].index_a == 0 && pairs[0].index_b == 1);
    return NULL;
}

static char *test_detect_three_all_overlap() {
    PhysicsObject objects[3];
    make_unit_cube(&objects[0], 1.0, 0.0, 0.0, 0.0);
    make_unit_cube(&objects[1], 1.0, 0.3, 0.0, 0.0);  /* overlaps [0] and [2] */
    make_unit_cube(&objects[2], 1.0, 0.6, 0.0, 0.0);  /* overlaps [1] */
    CollisionPair pairs[16];
    int n = collision_detect(objects, 3, pairs, 16);
    mu_assert("three clustered cubes → 3 pairs", n == 3);
    return NULL;
}

static char *test_detect_no_duplicates() {
    /* Verify each unordered pair appears at most once. */
    PhysicsObject objects[3];
    make_unit_cube(&objects[0], 1.0, 0.0, 0.0, 0.0);
    make_unit_cube(&objects[1], 1.0, 0.3, 0.0, 0.0);
    make_unit_cube(&objects[2], 1.0, 0.6, 0.0, 0.0);
    CollisionPair pairs[64];
    int n = collision_detect(objects, 3, pairs, 64);

    for (int i = 0; i < n; i++) {
        /* index_a must be strictly less than index_b */
        mu_assert("index_a < index_b", pairs[i].index_a < pairs[i].index_b);
        /* No duplicate of this pair later in the list */
        for (int j = i + 1; j < n; j++) {
            mu_assert("no duplicate pairs",
                      !(pairs[j].index_a == pairs[i].index_a &&
                        pairs[j].index_b == pairs[i].index_b));
        }
    }
    return NULL;
}

static char *test_detect_determinism() {
    /*
     * Two runs on identical input must find the same SET of pairs.
     * Element-wise order is not guaranteed: OpenMP merges per-thread results
     * in whichever order threads finish, so order can vary between runs.
     */
    PhysicsObject objects[3];
    make_unit_cube(&objects[0], 1.0, 0.0, 0.0, 0.0);
    make_unit_cube(&objects[1], 1.0, 0.3, 0.0, 0.0);
    make_unit_cube(&objects[2], 1.0, 0.6, 0.0, 0.0);

    CollisionPair pairs1[16], pairs2[16];
    int n1 = collision_detect(objects, 3, pairs1, 16);
    int n2 = collision_detect(objects, 3, pairs2, 16);

    mu_assert("same count on second run", n1 == n2);

    /* Every pair in run 1 must appear somewhere in run 2. */
    for (int i = 0; i < n1; i++) {
        int found = 0;
        for (int j = 0; j < n2; j++) {
            if (pairs1[i].index_a == pairs2[j].index_a &&
                    pairs1[i].index_b == pairs2[j].index_b) {
                found = 1;
                break;
            }
        }
        mu_assert("every pair from run 1 appears in run 2", found);
    }
    return NULL;
}

static const TestCase tests[] = {
    {"sat_separated",           test_sat_separated},
    {"sat_overlapping",         test_sat_overlapping},
    {"sat_touching_face",       test_sat_touching_face},
    {"detect_empty",            test_detect_empty},
    {"detect_single",           test_detect_single},
    {"detect_two_separated",    test_detect_two_separated},
    {"detect_two_overlapping",  test_detect_two_overlapping},
    {"detect_three_one_overlap",test_detect_three_one_overlap},
    {"detect_three_all_overlap",test_detect_three_all_overlap},
    {"detect_no_duplicates",    test_detect_no_duplicates},
    {"detect_determinism",      test_detect_determinism},
};

int main(void) {
    int failed = run_suite("Collision Detection", tests,
                           sizeof(tests) / sizeof(tests[0]));
    return finish_suite(failed);
}
