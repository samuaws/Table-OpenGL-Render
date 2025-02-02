#include "Camera.h"
#include <cmath>

Camera::Camera(Vector3 position) : position(position), rotation(0.0f, 0.0f, 0.0f), fov(45.0f), aspectRatio(1080.0f / 720.0f) {}

void Camera::move(const Vector3& direction, float deltaTime) {
    float speed = 3.0f * deltaTime;
    position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;
}

void Camera::rotate(const Vector3& rotationDelta) {
    rotation.x += rotationDelta.x * 0.1f;
    rotation.y += rotationDelta.y * 0.1f;
}

const Vector3& Camera::getPosition() const {
    return position;
}

Mat4 Camera::getViewMatrix() const {
    float cosPitch = cos(rotation.x);
    float sinPitch = sin(rotation.x);
    float cosYaw = cos(rotation.y);
    float sinYaw = sin(rotation.y);

    Vector3 front = Vector3(
        cosYaw * cosPitch,
        sinPitch,
        sinYaw * cosPitch
    );

    front = front * -1.0f; // Reverse direction
    return Mat4::lookAt(position.x, position.y, position.z,
        position.x + front.x, position.y + front.y, position.z + front.z,
        0.0f, 1.0f, 0.0f);
}

Mat4 Camera::getProjectionMatrix() const {
    return Mat4::perspective(fov * (3.14159f / 180.0f), aspectRatio, 0.1f, 100.0f);
}
