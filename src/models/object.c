/**
 * @file object.c
 * @brief Implementation of PhysicsObject initialization.
 *
 * @author Steven Kight
 */

#include "object.h"

void object_init(PhysicsObject *obj, double mass, double x, double y, double z) {
    obj->mass = mass;
    obj->position.x = x;
    obj->position.y = y;
    obj->position.z = z;
}
