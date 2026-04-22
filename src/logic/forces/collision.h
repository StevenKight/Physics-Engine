/**
 * @file collision.h
 * @brief Inelastic collision response force between two physics objects.
 *
 * Exposes a single function for computing and applying the velocity change
 * produced by a pairwise inelastic collision. The response is applied along
 * the collision normal (centre-to-centre direction); tangential velocity
 * components are unchanged.
 *
 * CONVEX GEOMETRY ONLY: intended to pair with the SAT-based collision
 * detection pipeline, which only produces valid pairs for convex meshes.
 *
 * @author Steven Kight
 */

#ifndef FORCES_COLLISION_H
#define FORCES_COLLISION_H

#include "../../models/object.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Apply inelastic collision response to one colliding pair.
 *
 * Projects both velocities onto the collision normal, computes post-collision
 * scalar velocities using:
 *
 *   v_a = (C_R * m_b * (u_b - u_a) + m_a * u_a + m_b * u_b) / (m_a + m_b)
 *   v_b = (C_R * m_a * (u_a - u_b) + m_a * u_a + m_b * u_b) / (m_a + m_b)
 *
 * and applies the delta back along the normal. No response is applied if the
 * objects are already separating along the normal.
 *
 * @param a           First colliding object (mutated in place).
 * @param b           Second colliding object (mutated in place).
 * @param restitution Coefficient of restitution C_R in [0, 1].
 *                    0 = perfectly inelastic (objects stick together).
 *                    1 = elastic (no kinetic energy lost).
 */
void inelastic_collision(PhysicsObject *a, PhysicsObject *b, double restitution);

#ifdef __cplusplus
}
#endif

#endif /* FORCES_COLLISION_H */
