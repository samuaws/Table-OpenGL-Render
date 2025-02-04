#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <glad/glad.h>


#include <iostream>

struct Mesh {
    GLuint VAO, VBO, EBO, textureID;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

class ModelLoader {
public:
    Mesh loadModel(const std::string& path, const std::string& texturePath);
};

#endif
