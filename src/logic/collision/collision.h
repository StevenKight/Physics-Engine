/**
 * @file collision.h
 * @brief Public interface for broad-phase + narrow-phase collision detection.
 *
 * collision_detect() is the single entry point. It sequences an octree
 * broad phase (AABB overlap) followed by a SAT narrow phase and writes the
 * confirmed colliding index pairs to a caller-allocated output buffer.
 *
 * CONVEX GEOMETRY ONLY: the SAT narrow phase is mathematically valid only for
 * convex meshes. Concave objects must be decomposed into convex pieces (e.g.
 * convex hull) before being passed to the simulation. Results are undefined
 * for non-convex geometry.
 *
 * This header must be included before octree.h and sat.h because both depend
 * on CollisionPair.
 *
 * @author Steven Kight
 */

#ifndef COLLISION_H
#define COLLISION_H

#include "../../models/object.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Indices of one confirmed colliding pair. Always index_a < index_b. */
typedef struct {
    int index_a;
    int index_b;
} CollisionPair;

/**
 * @brief Detect all colliding pairs among count objects.
 *
 * Phase 1 (broad): builds an octree over the objects' world-space AABBs and
 *   collects candidate pairs that share at least one octree leaf.
 * Phase 2 (narrow): runs SAT on each candidate and retains only true
 *   intersections.
 *
 * Objects with vertex_count == 0 are treated as points in the broad phase and
 * skipped by the narrow phase (they produce no confirmed pairs).
 *
 * @param objects    Flat array of PhysicsObject. Must not be NULL.
 * @param count      Number of objects.
 * @param pairs_out  Caller-allocated buffer for confirmed collision pairs.
 * @param max_pairs  Capacity of pairs_out; extra pairs are silently dropped.
 * @return           Number of confirmed collision pairs written to pairs_out.
 */
int collision_detect(const PhysicsObject *objects, int count,
                     CollisionPair *pairs_out, int max_pairs);

#ifdef __cplusplus
}
#endif

#endif /* COLLISION_H */
