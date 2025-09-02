#pragma once

#include <cmath>

#include <constructs.hpp>

namespace state {

struct Object {
    // Linear Motion Properties
    Vec3 position;
    Vec3 velocity;
    Vec3 acceleration;
    Vec3 force_accumulator; // Accumulates forces over a frame
    double mass = 1.0;

    // Rotational Motion Properties
    Quaternion orientation; // Current rotation
    Vec3 angular_velocity;
    Vec3 torque_accumulator; // Accumulates torques over a frame

    // Physical Properties
    Vec3 scale;
    Vec3 center_of_mass;   // Offset from position in local space
    Mat3x3 inertia_tensor; // How the object resists rotation

    // Collision Properties
    double restitution = 0.5; // Bounciness
    double friction = 0.5;
};

inline void verlet_integration(state::Object &object, state::Vec3 new_a,
                               float delta_t) {
    // Calculate new values
    state::Vec3 new_x = object.position + object.velocity * delta_t +
                        1 / 2 * object.acceleration * pow(delta_t, 2);
    state::Vec3 new_v =
        object.velocity + (object.acceleration + new_a) / 2 * delta_t;

    // Update object values
    object.position = new_x;
    object.velocity = new_v;
    object.acceleration = new_a;
}

} // namespace state