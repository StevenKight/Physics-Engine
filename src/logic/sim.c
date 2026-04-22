/**
 * @file sim.c
 * @brief Top-level simulation loop: force accumulation and Verlet integration.
 *
 * @author Steven Kight
 * @date 2026-04-14
 */

#include "sim.h"

#include "forces/gravity.h"
#include "forces/collision.h"
#include "collision/collision.h"
#include "../models/object.h"

#include <stdlib.h>
#include <omp.h>

void sim_run(PhysicsObject *objects, int count, double time_step,
             int num_steps) {
    Vec3 *forces = malloc(count * sizeof(Vec3));
    
    /* Worst-case pair count: every object collides with every other. */
    int max_pairs = count * (count - 1) / 2;
    CollisionPair *pairs = malloc((max_pairs > 0 ? max_pairs : 1) * sizeof(CollisionPair));

    for (int tick = 0; tick < num_steps; tick++) {
        // Compute net gravitational force on each body.
        newtonian_gravity(objects, count, forces);

        for (int i = 0; i < count; i++) {
            objects[i].force = forces[i];
        }

        int n = collision_detect(objects, count, pairs, max_pairs);

        // TODO: Optimize the list of collisions for parallel work
        for (int i = 0; i < n; i++) {
            inelastic_collision(&objects[pairs[i].index_a],
                               &objects[pairs[i].index_b],
                               0.5);
        }

        // Advance each body one Velocity Verlet step; resets obj->force to zero.
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < count; i++) {
            object_step(&objects[i], time_step);
        }
    }

    free(forces);
    free(pairs);
}
