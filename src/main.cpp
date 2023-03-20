#include <iostream>
#include <Shader.h>
#include <Model.h>
#include <Renderer.h>

int main() {

    std::string_view vShaderPath = "..\\..\\src\\shaders\\PhongShader.vs";
    std::string_view fShaderPath = "..\\..\\src\\shaders\\PhongShader.fs";
    std::string modelPath = "..\\..\\assets\\mary\\Marry.obj";

    Renderer renderer(vShaderPath, fShaderPath, modelPath);

    renderer.render();

    return 0;
}
