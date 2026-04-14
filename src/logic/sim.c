/**
 * @file sim.c
 * @brief Top-level simulation loop: force accumulation and Verlet integration.
 *
 * @author Steven Kight
 * @date 2026-04-14
 */

#include "sim.h"

#include "forces/gravity.h"
#include "../models/object.h"

#include <stdlib.h>

void sim_run(PhysicsObject *objects, int count, double time_step, int num_steps) {
    Vec3 *forces = malloc(count * sizeof(Vec3));

    for (int tick = 0; tick < num_steps; tick++) {
        // Compute net gravitational force on each body and accumulate into obj->force.
        newtonian_gravity(objects, count, forces);
        for (int i = 0; i < count; i++) {
            objects[i].force = forces[i];
        }

        // Advance each body one Velocity Verlet step; resets obj->force to zero.
        for (int i = 0; i < count; i++) {
            step(&objects[i], time_step);
        }
    }

    free(forces);
}
