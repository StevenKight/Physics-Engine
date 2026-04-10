/**
 * @file object.h
 * @brief Basic physics object model with mass and 3D position.
 *
 * @author Steven Kight
 */

#ifndef OBJECT_H
#define OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A 3D vector with double-precision components.
 */
typedef struct {
    double x;
    double y;
    double z;
} Vec3;

/**
 * @brief A basic physics object with mass and position.
 */
typedef struct {
    double mass;
    Vec3 position;
    Vec3 velocity;
    Vec3 acceleration;
    Vec3 force;
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

#ifdef __cplusplus
}
#endif

#endif // OBJECT_H
