#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

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

GLuint tableVAO, tableVBO, shaderProgram;
Mat4 view, projection;

// Setup table top (3D rectangular prism)
void setupTable()
{
    float halfWidth = TABLE_WIDTH * 0.5f;
    float halfLength = TABLE_LENGTH * 0.5f;
    float thickness = TABLE_HEIGHT;

    float vertices[] = {
        // Top face
        -halfWidth, thickness, -halfLength,   halfWidth, thickness, -halfLength,   halfWidth, thickness,  halfLength,
        -halfWidth, thickness, -halfLength,   halfWidth, thickness,  halfLength,  -halfWidth, thickness,  halfLength,

        // Bottom face
        -halfWidth, 0.0f, -halfLength,   halfWidth, 0.0f, -halfLength,   halfWidth, 0.0f,  halfLength,
        -halfWidth, 0.0f, -halfLength,   halfWidth, 0.0f,  halfLength,  -halfWidth, 0.0f,  halfLength,

        // Front face
        -halfWidth, 0.0f, -halfLength,   halfWidth, 0.0f, -halfLength,   halfWidth, thickness, -halfLength,
        -halfWidth, 0.0f, -halfLength,   halfWidth, thickness, -halfLength,  -halfWidth, thickness, -halfLength,

        // Back face
        -halfWidth, 0.0f,  halfLength,   halfWidth, 0.0f,  halfLength,   halfWidth, thickness,  halfLength,
        -halfWidth, 0.0f,  halfLength,   halfWidth, thickness,  halfLength,  -halfWidth, thickness,  halfLength,

        // Left face
        -halfWidth, 0.0f, -halfLength,  -halfWidth, 0.0f,  halfLength,  -halfWidth, thickness,  halfLength,
        -halfWidth, 0.0f, -halfLength,  -halfWidth, thickness,  halfLength, -halfWidth, thickness, -halfLength,

        // Right face
        halfWidth, 0.0f, -halfLength,  halfWidth, 0.0f,  halfLength,  halfWidth, thickness,  halfLength,
        halfWidth, 0.0f, -halfLength,  halfWidth, thickness,  halfLength,  halfWidth, thickness, -halfLength
    };

    glGenVertexArrays(1, &tableVAO);
    glGenBuffers(1, &tableVBO);
    glBindVertexArray(tableVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tableVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Render the table
void drawTable()
{
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view.m);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection.m);

    glBindVertexArray(tableVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
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
    view = Mat4::lookAt(0.0f, 2.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    projection = Mat4::perspective(3.14159f / 4.0f, 1080.0f / 720.0f, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawTable();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

