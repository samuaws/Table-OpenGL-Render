#ifndef SCENE_CONTROLLER_INPUT_H
#define SCENE_CONTROLLER_INPUT_H

#include "Camera.h"
#include <GLFW/glfw3.h>

class SceneControllerInput {
public:
    SceneControllerInput(Camera* camera, GLFWwindow* window);
    void processInput(float deltaTime);

private:
    Camera* camera;
    GLFWwindow* window;
    double lastMouseX, lastMouseY;
    bool firstMouseMove;
};

#endif
