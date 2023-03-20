#pragma once

#include <glad/glad.h>
#include <Shader.h>
#include <Mesh.h>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <iostream>

class Model
{
public:
    /*  函数   */
    Model(std::string const &path, int shadowmapid, int pointshadowmapid)
    :shadowMapID(shadowmapid), pointShadowMapID(pointshadowmapid)
    {
        loadModel(path);
    }
    void Draw(ShaderProgram &shader, bool isShadow);

private:
    /*  模型数据  */
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded; // 储存所有已载入的textures
    int shadowMapID;
    int pointShadowMapID;
    /*  函数   */
    void loadModel(std::string const &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};