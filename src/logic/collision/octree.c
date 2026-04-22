/**
 * @file octree.c
 * @brief Octree node-pool implementation.
 *
 * @author Steven Kight
 */

#include "octree.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/* Internal helpers                                                      */
/* ------------------------------------------------------------------ */

static int alloc_node(OctreePool *pool, AABB bounds, int depth) {
    if (pool->node_count >= OCTREE_MAX_NODES)
        return OCTREE_NULL;

    int idx = pool->node_count++;
    OctreeNode *n = &pool->nodes[idx];
    memset(n, 0, sizeof(*n));
    n->bounds     = bounds;
    n->depth      = depth;
    n->is_leaf    = 1;
    n->body_count = 0;
    for (int i = 0; i < 8; i++)
        n->children[i] = OCTREE_NULL;
    return idx;
}

/* Grow an AABB to encompass the given corner point. */
static void expand_to(AABB *box, Vec3 p) {
    if (p.x < box->min.x) box->min.x = p.x;
    if (p.y < box->min.y) box->min.y = p.y;
    if (p.z < box->min.z) box->min.z = p.z;
    if (p.x > box->max.x) box->max.x = p.x;
    if (p.y > box->max.y) box->max.y = p.y;
    if (p.z > box->max.z) box->max.z = p.z;
}

/* ------------------------------------------------------------------ */
/* Insertion                                                             */
/* ------------------------------------------------------------------ */

/*
 * Recursively insert body_idx into all leaves of node_idx whose bounds
 * overlap body_aabb. On capacity overflow, the leaf is split and existing
 * bodies redistributed before the new body is inserted.
 *
 * objects is needed during splits to recompute AABBs for redistributed bodies.
 */
static void insert_body(OctreePool *pool, int node_idx,
                         int body_idx, AABB body_aabb,
                         const PhysicsObject *objects) {
    if (!aabb_overlaps(pool->nodes[node_idx].bounds, body_aabb))
        return;

    OctreeNode *node = &pool->nodes[node_idx];

    if (node->is_leaf) {
        if (node->body_count < OCTREE_LEAF_CAPACITY ||
                node->depth >= OCTREE_MAX_DEPTH) {
            /* Store directly (at max depth we may exceed capacity, but the
               body count stays finite since it is bounded by the object count). */
            if (node->body_count < OCTREE_LEAF_CAPACITY)
                node->body_indices[node->body_count++] = body_idx;
            return;
        }

        /* Capacity exceeded and room to split — create 8 children. */
        AABB child_bounds[8];
        aabb_split_octants(node->bounds, child_bounds);

        int saved[OCTREE_LEAF_CAPACITY];
        int saved_count = node->body_count;
        memcpy(saved, node->body_indices, saved_count * sizeof(int));

        for (int c = 0; c < 8; c++) {
            int ci = alloc_node(pool, child_bounds[c], node->depth + 1);
            pool->nodes[node_idx].children[c] = ci;
        }
        pool->nodes[node_idx].is_leaf    = 0;
        pool->nodes[node_idx].body_count = 0;

        /* Redistribute existing bodies into children. */
        for (int k = 0; k < saved_count; k++) {
            AABB saved_aabb = aabb_from_object(&objects[saved[k]]);
            for (int c = 0; c < 8; c++) {
                int ci = pool->nodes[node_idx].children[c];
                if (ci != OCTREE_NULL)
                    insert_body(pool, ci, saved[k], saved_aabb, objects);
            }
        }
    }

    /* Insert the new body into all overlapping children (internal node path). */
    if (!pool->nodes[node_idx].is_leaf) {
        for (int c = 0; c < 8; c++) {
            int ci = pool->nodes[node_idx].children[c];
            if (ci != OCTREE_NULL)
                insert_body(pool, ci, body_idx, body_aabb, objects);
        }
    }
}

/* ------------------------------------------------------------------ */
/* Public: octree_build                                                  */
/* ------------------------------------------------------------------ */

void octree_build(OctreePool *pool, const PhysicsObject *objects, int count) {
    pool->node_count = 0;

    if (count == 0) {
        alloc_node(pool, (AABB){{0,0,0},{0,0,0}}, 0);
        return;
    }

    /* Compute world AABB spanning all object meshes. */
    AABB world = aabb_from_object(&objects[0]);
    for (int i = 1; i < count; i++) {
        AABB b = aabb_from_object(&objects[i]);
        expand_to(&world, b.min);
        expand_to(&world, b.max);
    }

    /* Small padding prevents degenerate zero-size octant splits. */
    world.min.x -= 1e-6; world.min.y -= 1e-6; world.min.z -= 1e-6;
    world.max.x += 1e-6; world.max.y += 1e-6; world.max.z += 1e-6;

    alloc_node(pool, world, 0);  /* root is always node 0 */

    for (int i = 0; i < count; i++) {
        AABB body_aabb = aabb_from_object(&objects[i]);
        insert_body(pool, 0, i, body_aabb, objects);
    }
}

/* ------------------------------------------------------------------ */
/* Pair collection                                                        */
/* ------------------------------------------------------------------ */

/* Linear-scan deduplication — O(P) per emit, suitable for small P.
   Replace with a hash set or sort+unique for large-N optimisation. */
static int pair_seen(const CollisionPair *pairs, int count, int ia, int ib) {
    for (int k = 0; k < count; k++) {
        if (pairs[k].index_a == ia && pairs[k].index_b == ib)
            return 1;
    }
    return 0;
}

static void emit_pair(CollisionPair *pairs_out, int *pair_count, int max_pairs,
                      int ia, int ib) {
    if (ia > ib) { int t = ia; ia = ib; ib = t; }  /* canonical: smaller index first */
    if (*pair_count >= max_pairs) return;
    if (pair_seen(pairs_out, *pair_count, ia, ib)) return;
    pairs_out[(*pair_count)++] = (CollisionPair){ .index_a = ia, .index_b = ib };
}

static void collect_leaf_pairs(const OctreePool *pool, int node_idx,
                                CollisionPair *pairs_out, int *pair_count,
                                int max_pairs) {
    const OctreeNode *node = &pool->nodes[node_idx];

    if (node->is_leaf) {
        /* All combinations of the k bodies in this leaf. */
        for (int i = 0; i < node->body_count; i++) {
            for (int j = i + 1; j < node->body_count; j++) {
                emit_pair(pairs_out, pair_count, max_pairs,
                          node->body_indices[i], node->body_indices[j]);
            }
        }
    } else {
        for (int c = 0; c < 8; c++) {
            if (node->children[c] != OCTREE_NULL)
                collect_leaf_pairs(pool, node->children[c],
                                   pairs_out, pair_count, max_pairs);
        }
    }
}

int octree_query_pairs(const OctreePool *pool, CollisionPair *pairs_out,
                       int max_pairs) {
    int pair_count = 0;
    if (pool->node_count > 0)
        collect_leaf_pairs(pool, 0, pairs_out, &pair_count, max_pairs);
    return pair_count;
}
