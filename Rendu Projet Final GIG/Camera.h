#ifndef CAMERA_H
#define CAMERA_H

#include "Vector3.h"
#include "Mat4.h"

class Camera {
public:
    Camera(Vector3 position);
    void move(const Vector3& direction, float deltaTime);
    void rotate(const Vector3& rotationDelta);
    const Vector3& getPosition() const;

    Mat4 getViewMatrix() const;
    Mat4 getProjectionMatrix() const;

private:
    Vector3 position;
    Vector3 rotation;
    float fov;
    float aspectRatio;
};

#endif
