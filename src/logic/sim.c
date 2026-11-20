/**
 * @file sim.c
 * @brief Top-level simulation loop: force accumulation and Verlet integration.
 *
 * @author Steven Kight
 * @date 2026-04-14
 */

#include "sim.h"

#include "../models/object.h"
#include "collision/collision.h"
#include "forces/collision.h"
#include "forces/gravity.h"

#include <omp.h>
#include <stdlib.h>

// Compute net gravitational force on each body at its current position.
static void apply_gravity(PhysicsObject *objects, int count, Vec3 *forces) {
    newtonian_gravity(objects, count, forces);

    for (int i = 0; i < count; i++) {
        objects[i].force = forces[i];
    }
}

void sim_run(PhysicsObject *objects, int count, double time_step,
             int num_steps) {
    Vec3 *forces = malloc(count * sizeof(Vec3));

    /* Worst-case pair count: every object collides with every other. */
    int max_pairs = count * (count - 1) / 2;
    CollisionPair *pairs =
        malloc((max_pairs > 0 ? max_pairs : 1) * sizeof(CollisionPair));

    // Velocity Verlet needs a_t before the first position update. The stored
    // acceleration can't be trusted (new objects start at zero), so compute it
    // from the current positions.
    apply_gravity(objects, count, forces);
    for (int i = 0; i < count; i++) {
        objects[i].acceleration = vec3_div(objects[i].force, objects[i].mass);
        objects[i].force = (Vec3){0.0, 0.0, 0.0};
    }

    for (int tick = 0; tick < num_steps; tick++) {
        int n = collision_detect(objects, count, pairs, max_pairs);

        // TODO: Optimize the list of collisions for parallel work
        for (int i = 0; i < n; i++) {
            inelastic_collision(&objects[pairs[i].index_a],
                                &objects[pairs[i].index_b], 0.5);
        }

// Half-step velocity and move each body to x_{t+dt} using a_t.
#pragma omp parallel for schedule(static)
        for (int i = 0; i < count; i++) {
            object_update_position(&objects[i], time_step);
        }

        // Forces must come from the new positions for a true Verlet step.
        apply_gravity(objects, count, forces);

// Finish the velocity update with a_{t+dt}; resets obj->force to zero.
#pragma omp parallel for schedule(static)
        for (int i = 0; i < count; i++) {
            object_update_velocity(&objects[i], time_step);
        }
    }

    free(forces);
    free(pairs);
}
