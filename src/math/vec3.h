/**
 * @file vec3.h
 * @brief 3D double-precision vector type and operations.
 *
 * @author Steven Kight
 */

#ifndef VEC3_H
#define VEC3_H

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
 * @brief Component-wise addition: result = a + b.
 */
Vec3 vec3_add(Vec3 a, Vec3 b);

/**
 * @brief Component-wise subtraction: result = a - b.
 */
Vec3 vec3_sub(Vec3 a, Vec3 b);

/**
 * @brief Scalar multiplication: result = v * s.
 */
Vec3 vec3_scale(Vec3 v, double s);

/**
 * @brief Scalar division: result = v / s.
 */
Vec3 vec3_div(Vec3 v, double s);

/**
 * @brief Dot product: result = a · b.
 */
double vec3_dot(Vec3 a, Vec3 b);

/**
 * @brief Cross product: result = a × b.
 */
Vec3 vec3_cross(Vec3 a, Vec3 b);

/**
 * @brief Euclidean magnitude: result = |v|.
 */
double vec3_magnitude(Vec3 v);

/**
 * @brief Unit vector in the direction of v: result = v / |v|.
 *
 * Returns a zero vector if |v| == 0.
 */
Vec3 vec3_normalize(Vec3 v);

#ifdef __cplusplus
}
#endif

#endif // VEC3_H
