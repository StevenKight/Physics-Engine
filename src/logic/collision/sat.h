/**
 * @file sat.h
 * @brief SAT narrow-phase collision detection using convex mesh geometry.
 *
 * @author Steven Kight
 */

#ifndef SAT_H
#define SAT_H

#include "../../models/object.h"
#include "collision.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Test a single candidate pair using the Separating Axis Theorem.
 *
 * Axes tested:
 *   1. Face normals of a (from face_indices, CCW winding).
 *   2. Face normals of b.
 *   3. Cross products of every edge of a with every edge of b
 *      (needed to detect edge-edge contact that face normals miss).
 *
 * If any axis produces non-overlapping projection intervals the shapes are
 * separated and the function returns 0 immediately (early exit).
 *
 * Both objects must have vertex_count > 0 and face_count > 0. If either has
 * no mesh the result is undefined.
 *
 * CONVEX GEOMETRY ONLY: SAT is only correct for convex meshes. Concave
 * geometry will produce false negatives (missed collisions). Pre-process
 * meshes to their convex hull before passing to the simulation.
 *
 * @param a  First object.
 * @param b  Second object.
 * @return   1 if the meshes intersect, 0 if they are separated.
 */
int sat_test_one(const PhysicsObject *a, const PhysicsObject *b);

/**
 * @brief Run SAT over an array of candidate pairs and write confirmed hits.
 *
 * Iterates candidates[0..num_candidates-1] sequentially with OpenMP
 * parallelism. Confirmed pairs are written to pairs_out up to max_pairs.
 *
 * @param objects        Flat array of PhysicsObject.
 * @param candidates     Broad-phase candidate pairs (from octree_query_pairs).
 * @param num_candidates Length of candidates[].
 * @param pairs_out      Output buffer for confirmed collisions.
 * @param out_count      In/out: current fill level of pairs_out.
 * @param max_pairs      Capacity of pairs_out.
 */
void sat_test_pairs(const PhysicsObject *objects,
                    const CollisionPair *candidates, int num_candidates,
                    CollisionPair *pairs_out, int *out_count, int max_pairs);

/* TODO:
 * -- Fortran optimisation hook (not implemented) --
 *
 * Future: the project_interval inner loop (dot product of ≤64 vertices onto
 * one axis) is a tight reduction ideal for Fortran auto-vectorisation with -O3.
 *
 * Signature reserved (bind(C, name="sat_project_f")):
 *   void sat_project_f(const double *verts, const int *count,
 *                      const double *axis,
 *                      double *min_out, double *max_out);
 *
 * -- CUDA optimisation hook (not implemented) --
 *
 * Future: batch all candidate pairs; upload flat world-space vertex arrays;
 * launch one thread per candidate pair; write bool hit[] back; filter on host.
 * Threshold mirrors GRAVITY_GPU_THRESHOLD pattern.
 *
 * Signature reserved:
 *   void sat_test_pairs_cuda(const PhysicsObject *objects,
 *                            const CollisionPair *candidates, int n,
 *                            CollisionPair *pairs_out, int *out_count,
 *                            int max_pairs);
 */

#ifdef __cplusplus
}
#endif

#endif /* SAT_H */
