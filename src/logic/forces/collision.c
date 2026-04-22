/**
 * @file collision.c
 * @brief Inelastic collision response applied along the collision normal.
 *
 * @author Steven Kight
 */

#include "collision.h"
#include "../../math/vec3.h"

void inelastic_collision(PhysicsObject *a, PhysicsObject *b, double restitution) {
    Vec3   delta = vec3_sub(b->position, a->position);
    double dist  = vec3_magnitude(delta);
    if (dist < 1e-10) return;  /* coincident centres — no well-defined normal */

    Vec3 n = vec3_scale(delta, 1.0 / dist);  /* unit collision normal */

    /* Scalar velocity components along the normal */
    double ua = vec3_dot(a->velocity, n);
    double ub = vec3_dot(b->velocity, n);

    /* Skip if objects are already separating along the normal */
    if (ua <= ub) return;

    double ma    = a->mass;
    double mb    = b->mass;
    double total = ma + mb;

    /* Post-collision velocities along the normal (inelastic formula) */
    double va = (restitution * mb * (ub - ua) + ma * ua + mb * ub) / total;
    double vb = (restitution * ma * (ua - ub) + ma * ua + mb * ub) / total;

    /* Apply the Δv directly to velocity. Collision impulses are instantaneous —
     * adding to obj->force would only affect the *next* Velocity Verlet step's
     * position update (VV uses a_old for x_new), so both objects would still
     * reach the same position this tick and gravity would diverge. */
    a->velocity = vec3_add(a->velocity, vec3_scale(n, va - ua));
    b->velocity = vec3_add(b->velocity, vec3_scale(n, vb - ub));
}
