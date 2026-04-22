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

/** Maximum vertices and triangular faces per convex mesh. */
#define PHYS_MAX_VERTICES 64
#define PHYS_MAX_FACES    32

/**
 * @brief A rigid body with mass, kinematics, an accumulated net force, and
 *        an optional convex collision mesh.
 *
 * Kinematic fields (mass … force) are at fixed offsets and unchanged from the
 * original layout. Geometry fields are appended at the end and default to zero
 * (vertex_count == 0 means no collision geometry assigned).
 *
 * Mesh vertices are stored in local (body) space centred at the origin.
 * face_indices[f] holds three vertex indices forming triangle f in CCW winding.
 */
typedef struct {
    /* --- Kinematic fields (offsets unchanged) --- */
    double mass;       /**< Mass of the object (kg). */
    Vec3 position;     /**< Position vector (m). */
    Vec3 velocity;     /**< Velocity vector (m/s). */
    Vec3 acceleration; /**< Acceleration from the previous step (m/s^2). */
    Vec3 force;        /**< Accumulated net force; reset to zero by step(). */

    /* --- Convex mesh geometry (local space) --- */
    int  vertex_count; /**< Number of valid entries in local_verts (0 = no mesh). */
    int  _pad0;        /**< Alignment padding — do not use. */
    Vec3 local_verts[PHYS_MAX_VERTICES]; /**< Vertex positions in local space. */
    int  face_count;   /**< Number of valid triangles in face_indices. */
    int  _pad1;        /**< Alignment padding — do not use. */
    int  face_indices[PHYS_MAX_FACES][3]; /**< Vertex index triples, CCW winding. */
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
void object_step(PhysicsObject *obj, double time_step);

#ifdef __cplusplus
}
#endif

#endif // OBJECT_H
