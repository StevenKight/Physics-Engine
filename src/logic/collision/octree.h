/**
 * @file octree.h
 * @brief Integer-indexed node-pool octree for broad-phase collision detection.
 *
 * The entire tree lives in a flat OctreePool array — no dynamic allocation,
 * no pointers inside nodes. Integer child indices let the pool be serialised
 * or uploaded to GPU memory with a single cudaMemcpy (future optimisation).
 *
 * Objects are inserted into every leaf whose bounds overlap their AABB, so
 * two spatially overlapping objects always share at least one leaf. Candidate
 * pairs are then collected by iterating leaves and testing all pairs within
 * each one, with deduplication so each unordered pair appears exactly once.
 *
 * @author Steven Kight
 */

#ifndef OCTREE_H
#define OCTREE_H

#include "aabb.h"
#include "collision.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OCTREE_MAX_NODES 8192
#define OCTREE_MAX_DEPTH 8
#define OCTREE_LEAF_CAPACITY 8 /* bodies per leaf before splitting */
#define OCTREE_NULL -1         /* sentinel: no child / empty slot */

/**
 * @brief A single node in the node-pool octree.
 *
 * children[0..7] are integer indices into the pool (OCTREE_NULL = absent).
 * body_indices[0..body_count-1] holds object indices only when is_leaf == 1.
 * A body can appear in multiple leaves (multi-AABB insertion).
 *
 * No pointers — the struct is flat and suitable for cudaMemcpy to device
 * memory.
 */
typedef struct {
    AABB bounds;
    int children[8];
    int body_indices[OCTREE_LEAF_CAPACITY];
    int body_count;
    int is_leaf;
    int depth;
} OctreeNode;

/**
 * @brief Pre-allocated node pool.
 *
 * nodes[0] is always the root. node_count is the next free slot index.
 * The caller owns the memory (stack, static, or caller-malloc).
 */
typedef struct {
    OctreeNode nodes[OCTREE_MAX_NODES];
    int node_count;
} OctreePool;

/**
 * @brief Build an octree from scratch over an array of objects.
 *
 * Resets the pool and inserts every object by its world-space AABB.
 * Objects with vertex_count == 0 are treated as points.
 *
 * @param pool     Output pool (caller-allocated, will be fully reset).
 * @param objects  Flat array of PhysicsObject.
 * @param count    Number of objects.
 */
void octree_build(OctreePool *pool, const PhysicsObject *objects, int count);

/**
 * @brief Collect all AABB-overlapping candidate pairs from the tree.
 *
 * Iterates every leaf and emits each unique (index_a < index_b) pair of
 * objects that share that leaf. Deduplicates via a linear scan so pairs
 * that appear in multiple leaves are emitted only once.
 *
 * @param pool       Tree built by octree_build().
 * @param pairs_out  Caller-allocated output buffer.
 * @param max_pairs  Capacity of pairs_out; extra pairs are silently dropped.
 * @return           Number of candidate pairs written.
 */
int octree_query_pairs(const OctreePool *pool, CollisionPair *pairs_out,
                       int max_pairs);

/* TODO:
 * -- CUDA optimisation hook (not implemented) --
 *
 * Future: upload pool->nodes (flat integer array, no pointer fixup) to device
 * memory via a single cudaMemcpy; launch one warp per node pair to walk the
 * tree in parallel and write pairs to a device-side buffer; copy back.
 *
 * Signature reserved:
 *   int octree_query_pairs_cuda(const OctreePool *pool,
 *                               CollisionPair *pairs_out, int max_pairs);
 */

#ifdef __cplusplus
}
#endif

#endif /* OCTREE_H */
