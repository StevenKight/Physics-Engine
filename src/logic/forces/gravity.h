/**
 * @file gravity.h
 * @brief Gravitational force calculations acting on physics objects.
 *
 * Exposes functions for computing gravitational forces between collections
 * of physics objects. Callers are responsible for pre-allocating the output
 * force array with one entry per object.
 *
 * @author Steven Kight
 * @date 2026-04-09
 */

#ifndef GRAVITY_H
#define GRAVITY_H

#include "../../models/object.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Computes the net Newtonian gravitational force vector on each body.
 *
 * Combines scalar force magnitudes with unit direction vectors per pair,
 * then row-sums to accumulate the net force on each body (see §Multi-Body Case):
 *
 *   Stage 1 — scalar magnitudes:  F[i,j]     = G(m_i m_j) / r²_safe        (N×N)
 *   Stage 2 — unit directions:    D_hat[i,j] = ΔP[i,j] / r[i,j]            (N×N×3)
 *   Stage 3 — force vectors:      F_vec[i,j] = F[i,j] ⊙ D_hat[i,j]         (N×N×3)
 *   Stage 4 — net per body:       F(i)       = Σ_j F_vec[i,j]
 *
 * @param objects    Pointer to an array of PhysicsObject. Must not be NULL.
 * @param count      Number of objects (N).
 * @param forces_out Pre-allocated array of @p count Vec3 values that will
 *                   receive the net gravitational force on each body (Newtons).
 *                   Must not be NULL.
 */
void newtonian_gravity(const PhysicsObject *objects, int count,
                       Vec3 *forces_out);

#ifdef __cplusplus
}
#endif

#endif // GRAVITY_H
