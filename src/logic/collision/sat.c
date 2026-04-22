/**
 * @file sat.c
 * @brief SAT narrow-phase implementation.
 *
 * All vector math delegates to the existing vec3.h operations.
 *
 * @author Steven Kight
 */

#include "sat.h"
#include "../../math/vec3.h"

/* ------------------------------------------------------------------ */
/* Internal helpers                                                      */
/* ------------------------------------------------------------------ */

static void project_interval(const Vec3 *verts, int count, Vec3 axis,
                               double *min_out, double *max_out) {
    double lo =  1e300, hi = -1e300;
    for (int i = 0; i < count; i++) {
        double p = vec3_dot(verts[i], axis);
        if (p < lo) lo = p;
        if (p > hi) hi = p;
    }
    *min_out = lo;
    *max_out = hi;
}

/* 1 = overlapping intervals, 0 = gap found (separating axis). */
static int intervals_overlap(double min_a, double max_a,
                               double min_b, double max_b) {
    return max_a >= min_b && max_b >= min_a;
}

/* Test one axis against both world-space vertex sets.
   Returns 0 if this axis separates the bodies (early-exit signal). */
static int test_axis(const Vec3 *wa, int na, const Vec3 *wb, int nb, Vec3 axis) {
    double min_a, max_a, min_b, max_b;
    project_interval(wa, na, axis, &min_a, &max_a);
    project_interval(wb, nb, axis, &min_b, &max_b);
    return intervals_overlap(min_a, max_a, min_b, max_b);
}

/* ------------------------------------------------------------------ */
/* Public: sat_test_one                                                  */
/* ------------------------------------------------------------------ */

int sat_test_one(const PhysicsObject *a, const PhysicsObject *b) {
    /* Build world-space vertex arrays on the stack (no heap allocation). */
    Vec3 wa[PHYS_MAX_VERTICES], wb[PHYS_MAX_VERTICES];

    for (int i = 0; i < a->vertex_count; i++)
        wa[i] = vec3_add(a->local_verts[i], a->position);
    for (int i = 0; i < b->vertex_count; i++)
        wb[i] = vec3_add(b->local_verts[i], b->position);

    /* --- Axes from face normals of a --- */
    for (int f = 0; f < a->face_count; f++) {
        Vec3 v0 = wa[a->face_indices[f][0]];
        Vec3 v1 = wa[a->face_indices[f][1]];
        Vec3 v2 = wa[a->face_indices[f][2]];
        Vec3 raw = vec3_cross(vec3_sub(v1, v0), vec3_sub(v2, v0));
        if (vec3_magnitude(raw) < 1e-10) continue;  /* degenerate face */
        Vec3 n = vec3_normalize(raw);
        if (!test_axis(wa, a->vertex_count, wb, b->vertex_count, n))
            return 0;
    }

    /* --- Axes from face normals of b --- */
    for (int f = 0; f < b->face_count; f++) {
        Vec3 v0 = wb[b->face_indices[f][0]];
        Vec3 v1 = wb[b->face_indices[f][1]];
        Vec3 v2 = wb[b->face_indices[f][2]];
        Vec3 raw = vec3_cross(vec3_sub(v1, v0), vec3_sub(v2, v0));
        if (vec3_magnitude(raw) < 1e-10) continue;
        Vec3 n = vec3_normalize(raw);
        if (!test_axis(wa, a->vertex_count, wb, b->vertex_count, n))
            return 0;
    }

    /* --- Axes from edge × edge cross products ---
       Needed for edge-edge contacts that face normals alone cannot detect
       (e.g., two boxes whose edges cross at an angle). */
    for (int fa = 0; fa < a->face_count; fa++) {
        for (int ea = 0; ea < 3; ea++) {
            Vec3 ea_raw = vec3_sub(wa[a->face_indices[fa][(ea + 1) % 3]],
                                   wa[a->face_indices[fa][ea]]);
            if (vec3_magnitude(ea_raw) < 1e-10) continue;
            Vec3 ea_dir = vec3_normalize(ea_raw);

            for (int fb = 0; fb < b->face_count; fb++) {
                for (int eb = 0; eb < 3; eb++) {
                    Vec3 eb_raw = vec3_sub(wb[b->face_indices[fb][(eb + 1) % 3]],
                                           wb[b->face_indices[fb][eb]]);
                    if (vec3_magnitude(eb_raw) < 1e-10) continue;
                    Vec3 eb_dir = vec3_normalize(eb_raw);

                    Vec3 cross_raw = vec3_cross(ea_dir, eb_dir);
                    if (vec3_magnitude(cross_raw) < 1e-10)
                        continue;  /* parallel edges — axis is degenerate */
                    Vec3 axis = vec3_normalize(cross_raw);

                    if (!test_axis(wa, a->vertex_count, wb, b->vertex_count, axis))
                        return 0;
                }
            }
        }
    }

    return 1;  /* no separating axis found — meshes intersect */
}

/* ------------------------------------------------------------------ */
/* Public: sat_test_pairs                                                */
/* ------------------------------------------------------------------ */

void sat_test_pairs(const PhysicsObject *objects,
                    const CollisionPair *candidates, int num_candidates,
                    CollisionPair *pairs_out, int *out_count, int max_pairs) {
    /*
     * Future CUDA hook:
     *   if (num_candidates > SAT_GPU_THRESHOLD) {
     *       sat_test_pairs_cuda(...); return;
     *   }
     */

    /* Collect per-thread results then merge to avoid lock contention. */
#ifdef _OPENMP
#pragma omp parallel
    {
        CollisionPair local_buf[1024];
        int local_count = 0;

#pragma omp for schedule(dynamic)
        for (int k = 0; k < num_candidates; k++) {
            int ia = candidates[k].index_a;
            int ib = candidates[k].index_b;

            /* Skip objects without meshes. */
            if (objects[ia].vertex_count == 0 || objects[ib].vertex_count == 0)
                continue;
            if (objects[ia].face_count == 0 || objects[ib].face_count == 0)
                continue;

            if (sat_test_one(&objects[ia], &objects[ib])) {
                if (local_count < 1024)
                    local_buf[local_count++] =
                        (CollisionPair){ .index_a = ia, .index_b = ib };
            }
        }

#pragma omp critical
        {
            for (int k = 0; k < local_count && *out_count < max_pairs; k++)
                pairs_out[(*out_count)++] = local_buf[k];
        }
    }
#else
    for (int k = 0; k < num_candidates; k++) {
        int ia = candidates[k].index_a;
        int ib = candidates[k].index_b;

        if (objects[ia].vertex_count == 0 || objects[ib].vertex_count == 0)
            continue;
        if (objects[ia].face_count == 0 || objects[ib].face_count == 0)
            continue;

        if (*out_count >= max_pairs) break;

        if (sat_test_one(&objects[ia], &objects[ib]))
            pairs_out[(*out_count)++] =
                (CollisionPair){ .index_a = ia, .index_b = ib };
    }
#endif
}
