/**
 * @file object.h
 * @brief PhysicsObject type and its initialisation and integration interface.
 *
 * @author Steven Kight
 */

#ifndef OBJECT_H
#define OBJECT_H

#include "../math/vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A rigid body with mass, kinematics, and an accumulated net force.
 *
 * Fields are updated each simulation tick by step(). Callers must accumulate
 * all forces into @c force before calling step(), which resets it to zero
 * afterwards.
 */
typedef struct {
    double mass;       /**< Mass of the object (kg). */
    Vec3 position;     /**< Position vector (m). */
    Vec3 velocity;     /**< Velocity vector (m/s). */
    Vec3 acceleration; /**< Acceleration from the previous step (m/s^2). */
    Vec3 force; /**< Accumulated net force for the current step (N). Reset to
                   zero by step(). */
} PhysicsObject;

/**
 * @brief Initialize a PhysicsObject with the given mass and position.
 *
 * @param obj    Pointer to the object to initialize.
 * @param mass   Mass of the object (kg).
 * @param x      Initial x position (m).
 * @param y      Initial y position (m).
 * @param z      Initial z position (m).
 */
void object_init(PhysicsObject *obj, double mass, double x, double y, double z);

/**
 * @brief Advance @p obj by one time step using Velocity Verlet integration.
 *
 * All forces must be accumulated into @c obj->force before this is called.
 * On return, position, velocity, and acceleration are updated and @c obj->force
 * is reset to zero.
 *
 * @param obj        Pointer to the object to advance.
 * @param time_step  Duration of the time step (s).
 */
void step(PhysicsObject *obj, double time_step);

#ifdef __cplusplus
}
#endif

#endif // OBJECT_H
