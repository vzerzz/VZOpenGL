#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Model.h>
#include <Shader.h>

const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

class Shadow{
public:
    Shadow();
    void RenderShadow(Model &model1, Model &model2, ShaderProgram &dirShader, ShaderProgram &pointShader);

public:
    GLuint depthMap, depthCubeMap;
    GLuint depthMapFBO, depthCubeMapFBO;
};