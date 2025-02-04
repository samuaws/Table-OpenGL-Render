#include "ModelLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint loadTexture(const std::string& path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

Mesh ModelLoader::loadModel(const std::string& path, const std::string& texturePath)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
        return {};
    }

    Mesh mesh;
    aiMesh* aiMesh = scene->mMeshes[0];

    for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        mesh.vertices.push_back(aiMesh->mVertices[i].x);
        mesh.vertices.push_back(aiMesh->mVertices[i].y);
        mesh.vertices.push_back(aiMesh->mVertices[i].z);

        mesh.vertices.push_back(aiMesh->mNormals[i].x);
        mesh.vertices.push_back(aiMesh->mNormals[i].y);
        mesh.vertices.push_back(aiMesh->mNormals[i].z);

        if (aiMesh->mTextureCoords[0])
        {
            mesh.vertices.push_back(aiMesh->mTextureCoords[0][i].x);
            mesh.vertices.push_back(aiMesh->mTextureCoords[0][i].y);
        }
        else
        {
            mesh.vertices.push_back(0.0f);
            mesh.vertices.push_back(0.0f);
        }
    }

    for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace face = aiMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            mesh.indices.push_back(face.mIndices[j]);
        }
    }

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    mesh.textureID = loadTexture(texturePath);

    return mesh;
}
