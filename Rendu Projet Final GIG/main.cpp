#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

// Simple 4x4 Matrix
struct Mat4
{
    float m[16];

    static Mat4 identity()
    {
        Mat4 result = {};
        result.m[0] = 1.0f; result.m[5] = 1.0f; result.m[10] = 1.0f; result.m[15] = 1.0f;
        return result;
    }

    static Mat4 perspective(float fov, float aspect, float near, float far)
    {
        Mat4 result = {};
        float tanHalfFov = tanf(fov * 0.5f);
        result.m[0] = 1.0f / (aspect * tanHalfFov);
        result.m[5] = 1.0f / tanHalfFov;
        result.m[10] = -(far + near) / (far - near);
        result.m[11] = -1.0f;
        result.m[14] = -(2.0f * far * near) / (far - near);
        return result;
    }

    static Mat4 lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
    {
        float fx = centerX - eyeX, fy = centerY - eyeY, fz = centerZ - eyeZ;
        float rlf = 1.0f / sqrtf(fx * fx + fy * fy + fz * fz);
        fx *= rlf; fy *= rlf; fz *= rlf;

        float sx = upY * fz - upZ * fy, sy = upZ * fx - upX * fz, sz = upX * fy - upY * fx;
        float rls = 1.0f / sqrtf(sx * sx + sy * sy + sz * sz);
        sx *= rls; sy *= rls; sz *= rls;

        float ux = fy * sz - fz * sy, uy = fz * sx - fx * sz, uz = fx * sy - fy * sx;

        Mat4 result = identity();
        result.m[0] = sx; result.m[4] = sy; result.m[8] = sz;
        result.m[1] = ux; result.m[5] = uy; result.m[9] = uz;
        result.m[2] = -fx; result.m[6] = -fy; result.m[10] = -fz;
        result.m[12] = -(sx * eyeX + sy * eyeY + sz * eyeZ);
        result.m[13] = -(ux * eyeX + uy * eyeY + uz * eyeZ);
        result.m[14] = fx * eyeX + fy * eyeY + fz * eyeZ;
        return result;
    }
};

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
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.m);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.m);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.m);

    glUniform3f(lightDirLoc, -0.5f, -1.0f, -0.3f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(objectColorLoc, 0.8f, 0.6f, 0.4f);

    glBindVertexArray(tableVAO);
    glDrawElements(GL_TRIANGLES, 36 + 24, GL_UNSIGNED_INT, 0); // Now includes legs
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
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.m);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.m);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.m);

    glUniform3f(lightDirLoc, -0.5f, -1.0f, -0.3f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(objectColorLoc, 0.8f, 0.6f, 0.4f); // Same as tabletop

    glBindVertexArray(legsVAO);
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0); // sIncreased index count to include all legs
}

GLuint groundVAO, groundVBO, groundEBO;
void setupGround()
{
    float groundWidth = 10.0f;
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





void drawScene()
{
    drawSkybox();
    drawGround();
    drawTable();
    drawLegs();
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
    setupSkybox();

    view = Mat4::lookAt(0.0f, 2.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
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

