#pragma once

namespace state
{

    struct Vec3
    {
        double x = 0, y = 0, z = 0;
    };

    struct Object
    {
        Vec3 center;
        Vec3 scale;
        Vec3 velocity;
        Vec3 acceleration;
        double mass = 1.0;
    };

}
