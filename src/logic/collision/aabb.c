/**
 * @file aabb.c
 * @brief AABB helper implementations.
 *
 * @author Steven Kight
 */

#include "aabb.h"

AABB aabb_from_object(const PhysicsObject *obj) {
    if (obj->vertex_count == 0) {
        /* No mesh — treat object as a point at its position. */
        return (AABB){ .min = obj->position, .max = obj->position };
    }

    Vec3 world = vec3_add(obj->local_verts[0], obj->position);
    AABB box = { .min = world, .max = world };

    for (int i = 1; i < obj->vertex_count; i++) {
        world = vec3_add(obj->local_verts[i], obj->position);

        if (world.x < box.min.x) box.min.x = world.x;
        if (world.y < box.min.y) box.min.y = world.y;
        if (world.z < box.min.z) box.min.z = world.z;

        if (world.x > box.max.x) box.max.x = world.x;
        if (world.y > box.max.y) box.max.y = world.y;
        if (world.z > box.max.z) box.max.z = world.z;
    }

    return box;
}

int aabb_overlaps(AABB a, AABB b) {
    return a.max.x >= b.min.x && b.max.x >= a.min.x &&
           a.max.y >= b.min.y && b.max.y >= a.min.y &&
           a.max.z >= b.min.z && b.max.z >= a.min.z;
}

void aabb_split_octants(AABB parent, AABB out[8]) {
    Vec3 centre = {
        (parent.min.x + parent.max.x) * 0.5,
        (parent.min.y + parent.max.y) * 0.5,
        (parent.min.z + parent.max.z) * 0.5,
    };

    /* Child i: bit0=x, bit1=y, bit2=z selects positive (max) or negative (min) half. */
    for (int i = 0; i < 8; i++) {
        out[i].min.x = (i & 1) ? centre.x : parent.min.x;
        out[i].max.x = (i & 1) ? parent.max.x : centre.x;

        out[i].min.y = (i & 2) ? centre.y : parent.min.y;
        out[i].max.y = (i & 2) ? parent.max.y : centre.y;

        out[i].min.z = (i & 4) ? centre.z : parent.min.z;
        out[i].max.z = (i & 4) ? parent.max.z : centre.z;
    }
}
