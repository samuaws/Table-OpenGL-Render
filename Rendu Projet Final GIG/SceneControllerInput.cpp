#include "SceneControllerInput.h"
#include <iostream>

SceneControllerInput::SceneControllerInput(Camera* camera, GLFWwindow* window)
    : camera(camera), window(window), lastMouseX(400), lastMouseY(300), firstMouseMove(true) {}

void SceneControllerInput::processInput(float deltaTime) {
    static bool wasRightMouseHeld = false;

    bool rightMouseHeld = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if (rightMouseHeld && !wasRightMouseHeld) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }

    Vector3 movement(0.0f, 0.0f, 0.0f);
    float moveSpeed = 5.0f * deltaTime; // Movement speed factor

    if (rightMouseHeld) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) movement.z -= moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) movement.z += moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) movement.x -= moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) movement.x += moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) movement.y += moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) movement.y -= moveSpeed;

        camera->move(movement, deltaTime);
    }

    if (rightMouseHeld) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float xOffset = static_cast<float>(mouseX - lastMouseX);
        float yOffset = static_cast<float>(lastMouseY - mouseY);
        lastMouseX = mouseX;
        lastMouseY = mouseY;

        camera->rotate(Vector3(yOffset, xOffset, 0.0f));
    }

    wasRightMouseHeld = rightMouseHeld;
}
