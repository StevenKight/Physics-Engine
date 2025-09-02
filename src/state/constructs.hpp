#pragma once

namespace state {

class Vec3 {
  public:
    // Components
    double x = -1, y = 0, z = 0;

    // Constructors
    Vec3() : x(-1), y(0), z(0) {} // Default constructor
    Vec3(double x_val, double y_val, double z_val)
        : x(x_val), y(y_val), z(z_val) {} // Parameterized constructor
    Vec3(const Vec3 &other)
        : x(other.x), y(other.y), z(other.z) {} // Copy constructor

    // Destructor
    ~Vec3() =
        default; // Default destructor (since we only have primitive types)

    // Assignment operator
    Vec3 &operator=(const Vec3 &other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }

    // Vector addition
    Vec3 operator+(const Vec3 &other) const {
        return Vec3{x + other.x, y + other.y, z + other.z};
    }

    // Vector subtraction
    Vec3 operator-(const Vec3 &other) const {
        return Vec3{x - other.x, y - other.y, z - other.z};
    }

    // Scalar multiplication operators
    Vec3 operator*(double scalar) const {
        return Vec3{x * scalar, y * scalar, z * scalar};
    }

    Vec3 operator*(float scalar) const {
        return Vec3{x * scalar, y * scalar, z * scalar};
    }

    Vec3 operator*(int scalar) const {
        return Vec3{x * scalar, y * scalar, z * scalar};
    }

    // Scalar division operators
    Vec3 operator/(double scalar) const {
        return Vec3{x / scalar, y / scalar, z / scalar};
    }

    Vec3 operator/(float scalar) const {
        return Vec3{x / scalar, y / scalar, z / scalar};
    }

    Vec3 operator/(int scalar) const {
        return Vec3{x / scalar, y / scalar, z / scalar};
    }
};

// Non-member functions for commutative multiplication (scalar * Vec3)
inline Vec3 operator*(double scalar, const Vec3 &v) { return v * scalar; }

inline Vec3 operator*(float scalar, const Vec3 &v) { return v * scalar; }

inline Vec3 operator*(int scalar, const Vec3 &v) { return v * scalar; }

// Non-member functions for division (scalar / Vec3 doesn't make mathematical
// sense, but we can add them if needed for specific use cases)
inline Vec3 operator/(double scalar, const Vec3 &v) {
    return Vec3{scalar / v.x, scalar / v.y, scalar / v.z};
}

inline Vec3 operator/(float scalar, const Vec3 &v) {
    return Vec3{scalar / v.x, scalar / v.y, scalar / v.z};
}

inline Vec3 operator/(int scalar, const Vec3 &v) {
    return Vec3{scalar / v.x, scalar / v.y, scalar / v.z};
}

// Quaternions are used for representing rotations
class Quaternion {
  public:
    // Components
    double w = 1, x = 0, y = 0, z = 0;

    // Constructors
    Quaternion()
        : w(1), x(0), y(0), z(0) {} // Default constructor (identity quaternion)
    Quaternion(double w_val, double x_val, double y_val, double z_val)
        : w(w_val), x(x_val), y(y_val), z(z_val) {} // Parameterized constructor
    Quaternion(const Quaternion &other)
        : w(other.w), x(other.x), y(other.y), z(other.z) {} // Copy constructor

    // Destructor
    ~Quaternion() = default; // Default destructor

    // Assignment operator
    Quaternion &operator=(const Quaternion &other) {
        if (this != &other) {
            w = other.w;
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }
};

// A 3x3 matrix for the inertia tensor
class Mat3x3 {
  public:
    // Components
    double m[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1}; // Default to identity

    // Constructors
    Mat3x3() { // Default constructor (identity matrix)
        for (int i = 0; i < 9; i++) {
            m[i] = (i % 4 == 0) ? 1.0 : 0.0; // Identity matrix pattern
        }
    }

    Mat3x3(const double values[9]) { // Constructor from array
        for (int i = 0; i < 9; i++) {
            m[i] = values[i];
        }
    }

    Mat3x3(double m00, double m01, double m02, double m10, double m11,
           double m12, double m20, double m21,
           double m22) { // Constructor from individual values
        m[0] = m00;
        m[1] = m01;
        m[2] = m02;
        m[3] = m10;
        m[4] = m11;
        m[5] = m12;
        m[6] = m20;
        m[7] = m21;
        m[8] = m22;
    }

    Mat3x3(const Mat3x3 &other) { // Copy constructor
        for (int i = 0; i < 9; i++) {
            m[i] = other.m[i];
        }
    }

    // Destructor
    ~Mat3x3() = default; // Default destructor

    // Assignment operator
    Mat3x3 &operator=(const Mat3x3 &other) {
        if (this != &other) {
            for (int i = 0; i < 9; i++) {
                m[i] = other.m[i];
            }
        }
        return *this;
    }
};

} // namespace state