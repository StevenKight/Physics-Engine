/**
 * @file aabb.h
 * @brief Axis-aligned bounding box helpers for broad-phase collision detection.
 *
 * All functions are pure geometry — no allocation, no global state.
 *
 * @author Steven Kight
 */

#ifndef AABB_H
#define AABB_H

#include "../../math/vec3.h"
#include "../../models/object.h"

#ifdef __cplusplus
extern "C" {
#endif

/** An axis-aligned bounding box defined by its minimum and maximum corners. */
typedef struct {
    Vec3 min;
    Vec3 max;
} AABB;

/**
 * @brief Compute the world-space AABB of a PhysicsObject's convex mesh.
 *
 * Translates each local_vert by obj->position and computes the component-wise
 * min and max. If vertex_count is 0, returns a zero-size AABB at position.
 *
 * @param obj  The object whose mesh is used.
 * @return     World-space AABB enclosing all vertices.
 */
AABB aabb_from_object(const PhysicsObject *obj);

/**
 * @brief Return 1 if two AABBs overlap on all three axes, 0 otherwise.
 *
 * Touching (shared face/edge/vertex) counts as overlapping.
 */
int aabb_overlaps(AABB a, AABB b);

/**
 * @brief Split an AABB into 8 equal octant children.
 *
 * out[i] receives the child whose bitmask index is i:
 *   bit0=x, bit1=y, bit2=z; set bit = positive half of that axis.
 *
 * @param parent  The AABB to split.
 * @param out     Array of 8 AABBs to fill.
 */
void aabb_split_octants(AABB parent, AABB out[8]);

#ifdef __cplusplus
}
#endif

#endif /* AABB_H */
