/**
 * @file object.c
 * @brief PhysicsObject initialization and Velocity Verlet integration step.
 *
 * @author Steven Kight
 */

#include "object.h"

void object_init(PhysicsObject *obj, double mass, double x, double y,
                 double z) {
    obj->mass = mass;

    obj->force.x = 0.0;
    obj->force.y = 0.0;
    obj->force.z = 0.0;

    obj->acceleration.x = 0.0;
    obj->acceleration.y = 0.0;
    obj->acceleration.z = 0.0;

    obj->velocity.x = 0.0;
    obj->velocity.y = 0.0;
    obj->velocity.z = 0.0;

    obj->position.x = x;
    obj->position.y = y;
    obj->position.z = z;

    // No collision mesh until one is attached
    obj->vertex_count = 0;
    obj->face_count = 0;
}

void object_update_position(PhysicsObject *obj, double time_step) {
    // v_{t+dt/2} = v_t + (1/2) * a_t * dt
    obj->velocity =
        vec3_add(obj->velocity, vec3_scale(obj->acceleration, 0.5 * time_step));

    // x_{t+dt} = x_t + v_{t+dt/2} * dt
    obj->position =
        vec3_add(obj->position, vec3_scale(obj->velocity, time_step));
}

void object_update_velocity(PhysicsObject *obj, double time_step) {
    // a_{t+dt} = F_net / m, with F_net measured at the new position
    obj->acceleration = vec3_div(obj->force, obj->mass);

    // v_{t+dt} = v_{t+dt/2} + (1/2) * a_{t+dt} * dt
    obj->velocity =
        vec3_add(obj->velocity, vec3_scale(obj->acceleration, 0.5 * time_step));

    // Reset force accumulator for next step
    obj->force = (Vec3){0.0, 0.0, 0.0};
}
