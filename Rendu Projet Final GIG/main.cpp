#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include "ModelLoader.h"
#include "stb_image.h"
#include "Mat4.h"



// Window resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Load shader source code from file
std::string loadShaderSource(const char* filename)
{
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Compile shader
GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}

// Create shader program
GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode = loadShaderSource(vertexPath);
    std::string fragmentCode = loadShaderSource(fragmentPath);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}



Mat4 setupCamera()
{
    float eyeX = 0.0f;
    float eyeY = 1.2f;  // Lowered the camera to 1.2 instead of 2.0
    float eyeZ = 4.5f;  // Moved slightly closer to the table

    float centerX = 0.0f;
    float centerY = 0.8f;  //  Looking slightly lower
    float centerZ = 0.0f;

    float upX = 0.0f;
    float upY = 1.0f;
    float upZ = 0.0f;

    return Mat4::lookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
}


// Scene variables
const float TABLE_WIDTH = 2.0f;
const float TABLE_LENGTH = 1.5f;
const float TABLE_HEIGHT = 0.1f;

GLuint tableVAO, tableVBO, tableEBO, shaderProgram;


Mat4 view, projection;
void setupTable()
{
    float halfWidth = TABLE_WIDTH * 0.5f;
    float halfLength = TABLE_LENGTH * 0.5f;
    float thickness = TABLE_HEIGHT;

    float vertices[] = {
        // Positions                 // Normals
        // Top face (Y-up)
        -halfWidth, thickness, -halfLength,   0.0f, 1.0f, 0.0f,
         halfWidth, thickness, -halfLength,   0.0f, 1.0f, 0.0f,
         halfWidth, thickness,  halfLength,   0.0f, 1.0f, 0.0f,
        -halfWidth, thickness,  halfLength,   0.0f, 1.0f, 0.0f,

        // Bottom face (Y-down)
        -halfWidth, 0.0f, -halfLength,   0.0f, -1.0f, 0.0f,
         halfWidth, 0.0f, -halfLength,   0.0f, -1.0f, 0.0f,
         halfWidth, 0.0f,  halfLength,   0.0f, -1.0f, 0.0f,
        -halfWidth, 0.0f,  halfLength,   0.0f, -1.0f, 0.0f,

        // Front face (Z-forward)
        -halfWidth, 0.0f, -halfLength,   0.0f, 0.0f, -1.0f,
         halfWidth, 0.0f, -halfLength,   0.0f, 0.0f, -1.0f,
         halfWidth, thickness, -halfLength,   0.0f, 0.0f, -1.0f,
        -halfWidth, thickness, -halfLength,   0.0f, 0.0f, -1.0f,

        // Back face (Z-backward)
        -halfWidth, 0.0f, halfLength,   0.0f, 0.0f, 1.0f,
         halfWidth, 0.0f, halfLength,   0.0f, 0.0f, 1.0f,
         halfWidth, thickness, halfLength,   0.0f, 0.0f, 1.0f,
        -halfWidth, thickness, halfLength,   0.0f, 0.0f, 1.0f,

        // Left face (X-left)
        -halfWidth, 0.0f, -halfLength,  -1.0f, 0.0f, 0.0f,
        -halfWidth, 0.0f,  halfLength,  -1.0f, 0.0f, 0.0f,
        -halfWidth, thickness,  halfLength,  -1.0f, 0.0f, 0.0f,
        -halfWidth, thickness, -halfLength,  -1.0f, 0.0f, 0.0f,

        // Right face (X-right)
         halfWidth, 0.0f, -halfLength,  1.0f, 0.0f, 0.0f,
         halfWidth, 0.0f,  halfLength,  1.0f, 0.0f, 0.0f,
         halfWidth, thickness,  halfLength,  1.0f, 0.0f, 0.0f,
         halfWidth, thickness, -halfLength,  1.0f, 0.0f, 0.0f
    };

    unsigned int indices[] = {
        // Top face
        0, 1, 2, 2, 3, 0,

        // Bottom face
        4, 5, 6, 6, 7, 4,

        // Front face
        8, 9, 10, 10, 11, 8,

        // Back face
        12, 13, 14, 14, 15, 12,

        // Left face
        16, 17, 18, 18, 19, 16,

        // Right face
        20, 21, 22, 22, 23, 20
    };

    glGenVertexArrays(1, &tableVAO);
    glGenBuffers(1, &tableVBO);
    glGenBuffers(1, &tableEBO);

    glBindVertexArray(tableVAO);

    glBindBuffer(GL_ARRAY_BUFFER, tableVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tableEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void drawTable()
{
    glUseProgram(shaderProgram);

    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
    GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    GLuint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    Mat4 model = Mat4::identity();
    model = Mat4::translate(model, -1.5f, 0.0f, -1.0f); // Move table to the right

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.m);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.m);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.m);

    glUniform3f(lightDirLoc, -0.5f, -1.0f, -0.3f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(objectColorLoc, 0.8f, 0.6f, 0.4f);

    glBindVertexArray(tableVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}



GLuint legsVAO, legsVBO, legsEBO;
void setupLegs()
{
    float halfWidth = TABLE_WIDTH * 0.5f;
    float halfLength = TABLE_LENGTH * 0.5f;
    float legHeight = 1.0f;
    float legThickness = 0.1f;

    float xOffset = halfWidth - legThickness * 0.5f;
    float zOffset = halfLength - legThickness * 0.5f;

    float vertices[] = {
        // Front-left leg
        -xOffset, 0.0f, -zOffset,   0.0f, 0.0f, 1.0f,
        -xOffset + legThickness, 0.0f, -zOffset,   0.0f, 0.0f, 1.0f,
        -xOffset + legThickness, -legHeight, -zOffset,   0.0f, 0.0f, 1.0f,
        -xOffset, -legHeight, -zOffset,   0.0f, 0.0f, 1.0f,

        // Front-right leg
         xOffset, 0.0f, -zOffset,   0.0f, 0.0f, 1.0f,
         xOffset - legThickness, 0.0f, -zOffset,   0.0f, 0.0f, 1.0f,
         xOffset - legThickness, -legHeight, -zOffset,   0.0f, 0.0f, 1.0f,
         xOffset, -legHeight, -zOffset,   0.0f, 0.0f, 1.0f,

         // Back-left leg
         -xOffset, 0.0f,  zOffset,   0.0f, 0.0f, 1.0f,
         -xOffset + legThickness, 0.0f,  zOffset,   0.0f, 0.0f, 1.0f,
         -xOffset + legThickness, -legHeight,  zOffset,   0.0f, 0.0f, 1.0f,
         -xOffset, -legHeight,  zOffset,   0.0f, 0.0f, 1.0f,

         // Back-right leg
          xOffset, 0.0f,  zOffset,   0.0f, 0.0f, 1.0f,
          xOffset - legThickness, 0.0f,  zOffset,   0.0f, 0.0f, 1.0f,
          xOffset - legThickness, -legHeight,  zOffset,   0.0f, 0.0f, 1.0f,
          xOffset, -legHeight,  zOffset,   0.0f, 0.0f, 1.0f,
    };

    unsigned int indices[] = {
        // Front-left leg
        0, 1, 2, 2, 3, 0,
        // Front-right leg
        4, 5, 6, 6, 7, 4,
        // Back-left leg
        8, 9, 10, 10, 11, 8,
        // Back-right leg
        12, 13, 14, 14, 15, 12
    };

    glGenVertexArrays(1, &legsVAO);
    glGenBuffers(1, &legsVBO);
    glGenBuffers(1, &legsEBO);

    glBindVertexArray(legsVAO);

    glBindBuffer(GL_ARRAY_BUFFER, legsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, legsEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void drawLegs()
{
    glUseProgram(shaderProgram);

    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
    GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    GLuint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    Mat4 model = Mat4::identity();
    model = Mat4::translate(model, -1.5f, 0.0f, -1.0f); // Move legs to match table

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.m);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.m);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.m);

    glUniform3f(lightDirLoc, -0.5f, -1.0f, -0.3f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(objectColorLoc, 0.8f, 0.6f, 0.4f); // Same as tabletop

    glBindVertexArray(legsVAO);
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
}


GLuint groundVAO, groundVBO, groundEBO;
void setupGround()
{
    float groundWidth = 14.0f;
    float groundLength = 10.0f;
    float groundThickness = 0.2f;
    float groundY = -1.2f; // Position below the table legs

    float halfWidth = groundWidth * 0.5f;
    float halfLength = groundLength * 0.5f;
    float halfThickness = groundThickness * 0.5f;

    float vertices[] = {
        // Positions                 // Normals
        // Top face (Y-up)
        -halfWidth, groundY, -halfLength,   0.0f, 1.0f, 0.0f,
         halfWidth, groundY, -halfLength,   0.0f, 1.0f, 0.0f,
         halfWidth, groundY,  halfLength,   0.0f, 1.0f, 0.0f,
        -halfWidth, groundY,  halfLength,   0.0f, 1.0f, 0.0f,

        // Bottom face (Y-down)
        -halfWidth, groundY - groundThickness, -halfLength,   0.0f, -1.0f, 0.0f,
         halfWidth, groundY - groundThickness, -halfLength,   0.0f, -1.0f, 0.0f,
         halfWidth, groundY - groundThickness,  halfLength,   0.0f, -1.0f, 0.0f,
        -halfWidth, groundY - groundThickness,  halfLength,   0.0f, -1.0f, 0.0f,

        // Front face (Z-forward)
        -halfWidth, groundY - groundThickness, -halfLength,   0.0f, 0.0f, -1.0f,
         halfWidth, groundY - groundThickness, -halfLength,   0.0f, 0.0f, -1.0f,
         halfWidth, groundY, -halfLength,   0.0f, 0.0f, -1.0f,
        -halfWidth, groundY, -halfLength,   0.0f, 0.0f, -1.0f,

        // Back face (Z-backward)
        -halfWidth, groundY - groundThickness,  halfLength,   0.0f, 0.0f, 1.0f,
         halfWidth, groundY - groundThickness,  halfLength,   0.0f, 0.0f, 1.0f,
         halfWidth, groundY,  halfLength,   0.0f, 0.0f, 1.0f,
        -halfWidth, groundY,  halfLength,   0.0f, 0.0f, 1.0f,

        // Left face (X-left)
        -halfWidth, groundY - groundThickness, -halfLength,  -1.0f, 0.0f, 0.0f,
        -halfWidth, groundY - groundThickness,  halfLength,  -1.0f, 0.0f, 0.0f,
        -halfWidth, groundY,  halfLength,  -1.0f, 0.0f, 0.0f,
        -halfWidth, groundY, -halfLength,  -1.0f, 0.0f, 0.0f,

        // Right face (X-right)
         halfWidth, groundY - groundThickness, -halfLength,  1.0f, 0.0f, 0.0f,
         halfWidth, groundY - groundThickness,  halfLength,  1.0f, 0.0f, 0.0f,
         halfWidth, groundY,  halfLength,  1.0f, 0.0f, 0.0f,
         halfWidth, groundY, -halfLength,  1.0f, 0.0f, 0.0f
    };

    unsigned int indices[] = {
        // Top face
        0, 1, 2, 2, 3, 0,
        // Bottom face
        4, 5, 6, 6, 7, 4,
        // Front face
        8, 9, 10, 10, 11, 8,
        // Back face
        12, 13, 14, 14, 15, 12,
        // Left face
        16, 17, 18, 18, 19, 16,
        // Right face
        20, 21, 22, 22, 23, 20
    };

    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glGenBuffers(1, &groundEBO);

    glBindVertexArray(groundVAO);

    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void drawGround()
{
    glUseProgram(shaderProgram);

    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
    GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    GLuint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    Mat4 model = Mat4::identity();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.m);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.m);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.m);

    glUniform3f(lightDirLoc, -0.5f, -1.0f, -0.3f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(objectColorLoc, 0.4f, 0.3f, 0.2f); // Brownish floor color

    glBindVertexArray(groundVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}


GLuint skyboxVAO, skyboxVBO, skyboxTexture, skyboxShaderProgram;

std::vector<std::string> skyboxFaces = {
    "textures/posx.jpg",  // Right (+X)
    "textures/negx.jpg",  // Left (-X)
    "textures/posy.jpg",  // Top (+Y)
    "textures/negy.jpg",  // Bottom (-Y)
    "textures/posz.jpg",  // Front (+Z)
    "textures/negz.jpg"   // Back (-Z)
};



GLuint loadCubemap(std::vector<std::string> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load cubemap texture: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
void setupSkybox()
{
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Load skybox shader
    skyboxShaderProgram = createShaderProgram("skybox_vertex.glsl", "skybox_fragment.glsl");

    // Load cubemap textures
    skyboxTexture = loadCubemap(skyboxFaces);
}


void drawSkybox()
{
    glDepthFunc(GL_LEQUAL);
    glUseProgram(skyboxShaderProgram);

    Mat4 viewNoTranslation = view;
    viewNoTranslation.m[12] = 0.0f;
    viewNoTranslation.m[13] = 0.0f;
    viewNoTranslation.m[14] = 0.0f;

    GLuint viewLoc = glGetUniformLocation(skyboxShaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(skyboxShaderProgram, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewNoTranslation.m);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.m);

    glBindVertexArray(skyboxVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}

ModelLoader modelLoader;
Mesh lampMesh;
GLuint lampShaderProgram;
float lampScale = 0.3f;


void setupLamp()
{
    lampMesh = modelLoader.loadModel("models/det3FBX.fbx", "textures/Diffuse.jpg");
    lampShaderProgram = createShaderProgram("lamp_vertex.glsl", "fragment_shader_tex.glsl");
}

void drawLamp()
{
    glUseProgram(lampShaderProgram);

    GLuint modelLoc = glGetUniformLocation(lampShaderProgram, "model");
    GLuint textureLoc = glGetUniformLocation(lampShaderProgram, "lampTexture");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, lampMesh.textureID);
    glUniform1i(textureLoc, 0);

    Mat4 model = Mat4::identity();
    model = Mat4::translate(model, 0.5f, TABLE_HEIGHT, 0.2f);
    model = Mat4::scale(model, 0.3f, 0.3f, 0.3f); // Scale the lamp

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.m);

    glBindVertexArray(lampMesh.VAO);
    glDrawElements(GL_TRIANGLES, lampMesh.indices.size(), GL_UNSIGNED_INT, 0);
}

GLuint ballVAO, ballVBO, ballEBO;
GLuint ballShaderProgram;

void setupBall()
{
    const int segments = 8;
    const int rings = 8;
    float radius = 0.2f;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= rings; i++)
    {
        float theta = (float)i / rings * 3.14159f;
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);

        for (int j = 0; j <= segments; j++)
        {
            float phi = (float)j / segments * 2.0f * 3.14159f;
            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            vertices.push_back(radius * x);
            vertices.push_back(radius * y);
            vertices.push_back(radius * z);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for (int i = 0; i < rings; i++)
    {
        for (int j = 0; j < segments; j++)
        {
            int first = (i * (segments + 1)) + j;
            int second = first + segments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    glGenVertexArrays(1, &ballVAO);
    glGenBuffers(1, &ballVBO);
    glGenBuffers(1, &ballEBO);

    glBindVertexArray(ballVAO);

    glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ballEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ballShaderProgram = createShaderProgram("ball_vertex.glsl", "ball_fragment.glsl");
}
void drawBall()
{
    glUseProgram(ballShaderProgram);

    GLuint modelLoc = glGetUniformLocation(ballShaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(ballShaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(ballShaderProgram, "projection");
    GLuint cameraPosLoc = glGetUniformLocation(ballShaderProgram, "cameraPos");

    Mat4 model = Mat4::identity();
    model = Mat4::translate(model, -1.0f, 0.5f, 0.5f);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.m);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.m);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.m);
    
    float eyeX = 0.0f;
    float eyeY = 1.2f;  // Lowered the camera to 1.2 instead of 2.0
    float eyeZ = 4.5f;  // Moved slightly closer to the table

    glUniform3f(cameraPosLoc, eyeX, eyeY, eyeZ); // Pass camera position

    glBindVertexArray(ballVAO);
    glDrawElements(GL_TRIANGLES, 288, GL_UNSIGNED_INT, 0);
}

const float WALL_WIDTH = 10.0f;
const float WALL_HEIGHT = 5.0f;
const float WALL_THICKNESS = 0.1f;
const float WINDOW_WIDTH = 1.5f;
const float WINDOW_HEIGHT = 1.2f;

GLuint wallVAO, wallVBO, wallEBO;

void setupWall()
{
    float hw = WALL_WIDTH * 0.5f;
    float hh = WALL_HEIGHT * 0.5f;
    float ht = WALL_THICKNESS * 0.5f;
    float hw_win = WINDOW_WIDTH * 1.5f;
    float hh_win = WINDOW_HEIGHT * 0.7f;

    float vertices[] = {
        // Left Part
        -hw, -hh, ht,  0.0f, 0.0f, 1.0f,
        -hw_win, -hh, ht,  0.0f, 0.0f, 1.0f,
        -hw, hh, ht,  0.0f, 0.0f, 1.0f,
        -hw_win, hh_win, ht,  0.0f, 0.0f, 1.0f,

        // Right Part
        hw_win, -hh, ht,  0.0f, 0.0f, 1.0f,
        hw, -hh, ht,  0.0f, 0.0f, 1.0f,
        hw_win, hh_win, ht,  0.0f, 0.0f, 1.0f,
        hw, hh, ht,  0.0f, 0.0f, 1.0f,

        // Bottom Middle Part
        -hw_win, -hh, ht,  0.0f, 0.0f, 1.0f,
        hw_win, -hh, ht,  0.0f, 0.0f, 1.0f,
        -hw_win, -hh_win, ht,  0.0f, 0.0f, 1.0f,
        hw_win, -hh_win, ht,  0.0f, 0.0f, 1.0f,

        // New Single Top Part (covers everything above the window)
        -hw, hh_win, ht,  0.0f, 0.0f, 1.0f,
        hw, hh_win, ht,  0.0f, 0.0f, 1.0f,
        -hw, hh, ht,  0.0f, 0.0f, 1.0f,
        hw, hh, ht,  0.0f, 0.0f, 1.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,  1, 3, 2,  // Left Part
        4, 5, 6,  5, 7, 6,  // Right Part
        8, 9, 10,  9, 11, 10, // Bottom Middle Part
        12, 13, 14,  13, 15, 14  // New Single Top Part
    };

    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);
    glGenBuffers(1, &wallEBO);

    glBindVertexArray(wallVAO);

    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void drawWall()
{
    glUseProgram(shaderProgram);

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
    GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    GLuint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");


    Mat4 model = Mat4::identity();
    model = Mat4::translate(model, 0.0f, WALL_HEIGHT * 0.5f - 1.0f, -2.5f); // Position the wall


    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.m);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.m);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.m);

    glUniform3f(lightDirLoc, -0.5f, -1.0f, -0.3f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(objectColorLoc, 0.4f, 0.3f, 0.2f); // Brownish floor color

    glBindVertexArray(wallVAO);
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
}





void drawScene()
{
    drawSkybox();
    drawGround();
    drawTable();
    drawLegs();
    drawLamp();
    drawBall();
    drawWall();
}

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1080, 720, "3D Scene", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    glViewport(0, 0, 1080, 720);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glEnable(GL_DEPTH_TEST);

    // Load shaders
    shaderProgram = createShaderProgram("vertex_shader.glsl", "fragment_shader.glsl");

    // Setup scene
    setupTable();
    setupLegs();
    setupGround();
    setupLamp();
    setupBall();
    setupWall();
    setupSkybox();

    view = setupCamera();
    projection = Mat4::perspective(3.14159f / 4.0f, 1080.0f / 720.0f, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawScene();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

