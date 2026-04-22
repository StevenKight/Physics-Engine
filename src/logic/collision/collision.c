/**
 * @file collision.c
 * @brief Entry point: sequences broad phase → narrow phase.
 *
 * @author Steven Kight
 */

#include "collision.h"
#include "octree.h"
#include "sat.h"

/*
 * OctreePool is ~850 KB. Using a static instance avoids blowing the default
 * 8 MB Linux stack while keeping the allocation simple (no caller-managed
 * heap). This means collision_detect is NOT thread-safe if called from
 * multiple threads simultaneously — acceptable for the serial sim loop.
 * For a multi-threaded future, replace with a caller-provided pool parameter.
 */
static OctreePool s_pool;

/* Heuristic: up to 8 broad-phase candidates per confirmed pair. */
#define MAX_CANDIDATES 8192

int collision_detect(const PhysicsObject *objects, int count,
                     CollisionPair *pairs_out, int max_pairs) {
    if (count <= 1 || !pairs_out || max_pairs <= 0)
        return 0;

    /* --- Phase 1: broad phase --- */
    octree_build(&s_pool, objects, count);

    static CollisionPair s_candidates[MAX_CANDIDATES];
    int n_candidates = octree_query_pairs(&s_pool, s_candidates, MAX_CANDIDATES);

    if (n_candidates == 0)
        return 0;

    /* --- Phase 2: narrow phase --- */
    int out_count = 0;
    sat_test_pairs(objects, s_candidates, n_candidates,
                   pairs_out, &out_count, max_pairs);

    return out_count;
}
