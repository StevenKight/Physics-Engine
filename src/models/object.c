/**
 * @file object.c
 * @brief PhysicsObject initialization and Velocity Verlet integration step.
 *
 * @author Steven Kight
 */

#include "object.h"

#include <math.h>

void object_init(PhysicsObject *obj, double mass, double x, double y, double z) {
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
}

void object_step(PhysicsObject *obj, double time_step) {
    // a_{t+dt} = F_net / m
    Vec3 next_a = vec3_div(obj->force, obj->mass);
    
    // v_{t+dt} = v_t + ((a_t + a_{t+dt}) / 2) * dt
    Vec3 next_v = vec3_add(
        obj->velocity,
        vec3_scale(
            vec3_div(
                vec3_add(obj->acceleration, next_a),
                2
            ),
            time_step
        )
    );

    // x_{t+dt} = x_t + v_t * dt + (1/2) * a_t * dt^2
    Vec3 next_position = vec3_add(
        vec3_add(
            obj->position,
            vec3_scale(obj->velocity, time_step)
        ),
        vec3_div(
            vec3_scale(obj->acceleration, pow(time_step, 2)),
            2
        )
    );

    // Write back updated state
    obj->position     = next_position;
    obj->velocity     = next_v;
    obj->acceleration = next_a;

    // Reset force accumulator for next step
    obj->force = (Vec3){0.0, 0.0, 0.0};
}