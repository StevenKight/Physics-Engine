/**
 * @file sim.h
 * @brief Top-level simulation loop interface.
 *
 * Drives a complete N-body simulation: applies forces (gravity) to every
 * object, then advances each object by one Velocity Verlet step, repeating
 * for the requested number of ticks.
 *
 * @author Steven Kight
 * @date 2026-04-14
 */

#ifndef SIM_H
#define SIM_H

#include "../models/object.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Run the simulation for a fixed number of time steps.
 *
 * Runs gravity, collision detection (internally — pairs are not exposed to the
 * caller), and Velocity Verlet integration each tick.
 *
 * Objects that have vertex/face geometry (vertex_count > 0) participate in
 * collision detection automatically. Objects without geometry are point masses
 * for gravity only.
 *
 * @param objects    Pointer to an array of PhysicsObject. Must not be NULL.
 * @param count      Number of objects (N).
 * @param time_step  Duration of each tick (s).
 * @param num_steps  Total number of ticks to simulate.
 */
void sim_run(PhysicsObject *objects, int count, double time_step,
             int num_steps);


#ifdef __cplusplus
}
#endif

#endif // SIM_H
