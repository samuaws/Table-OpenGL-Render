#ifndef VECTOR3_H
#define VECTOR3_H

struct Vector3 {
    float x, y, z;

    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}  //  Added Default Constructor
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}  //  Constructor Overload

    Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
    Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
    Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
};

#endif
