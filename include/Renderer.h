#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#include <Camera.h>
#include <Model.h>
#include <Light.h>
#include <Shadow.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Renderer
{
public:
    Renderer(std::string_view vShaderPath_, std::string_view fShaderPath_, std::string modelPath_);
    
    void render();

private:
    std::string_view vShaderPath;
    std::string_view fShaderPath;
    std::string modelPath;
};