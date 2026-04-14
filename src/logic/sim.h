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
 * Each tick:
 *   1. Computes the net force on every body and accumulates it
 *      into each object's @c force field.
 *   2. Calls step() on every object to advance position, velocity, and
 *      acceleration via Velocity Verlet integration and reset @c force to zero.
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
