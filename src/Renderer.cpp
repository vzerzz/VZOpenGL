#include "Renderer.h"

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float xoffset = 0.0f;
float yoffset = 0.0f;
// timing
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

Renderer::Renderer(std::string_view vShaderPath_, std::string_view fShaderPath_, std::string modelPath_)
    : vShaderPath(vShaderPath_), fShaderPath(fShaderPath_), modelPath(modelPath_)
{
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    //设置窗口维度
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

//监听鼠标移动事件
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    //防止第一次获取焦点的时候摄像机突然跳一下(鼠标移动进窗口的那一刻，鼠标回调函数就会被调用)
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    //计算鼠标距上一帧的偏移量
    xoffset = xpos - lastX;
    yoffset = lastY - ypos; //相反的，y坐标是从底部往顶部依次增大的

    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void Renderer::render()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "BlinnPhong", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to creat window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    stbi_set_flip_vertically_on_load(false);
    glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui context
    const char *glsl_version = "#version 130";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    // Our state
    bool dirLight = true;
    bool pointLight = true;
    bool spotLight = true;
    ImVec4 bgColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 floorColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    float modelPos[3] = {0.0f, 0.0f, 0.0f};
    float modelScale[3] = {0.5f, 0.5f, 0.5f};
    float lightDir[3] = {2.0f, -2.0f, 2.0f};
    float lightPos[3] = {1.0f, 3.0f, 2.0f};
    int shadowType = 0;
    
    Shadow shadow;

    ShaderProgram shader_(vShaderPath, fShaderPath);
    ShaderProgram lightShader("..\\..\\src\\shaders\\light.vs", "..\\..\\src\\shaders\\light.fs");
    Model model_(modelPath, shadow.depthMap, shadow.depthCubeMap);
    Model floor_("..\\..\\assets\\floor\\floor.obj", shadow.depthMap, shadow.depthCubeMap);
    Light light_;
    Light dirlight_;
    ShaderProgram depthShader("..\\..\\src\\shaders\\DepthShader.vs", "..\\..\\src\\shaders\\DepthShader.fs");
    ShaderProgram pointDepthShader("..\\..\\src\\shaders\\PointDepthShader.vs", "..\\..\\src\\shaders\\PointDepthShader.fs", "..\\..\\src\\shaders\\PointDepthShader.gs");

    while (!glfwWindowShouldClose(window)) // GLFW退出前一直运行
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hello");

        ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
        ImGui::Checkbox("DirectLight", &dirLight);
        ImGui::SameLine();
        ImGui::Checkbox("PointLight", &pointLight);
        ImGui::SameLine();
        ImGui::Checkbox("SpotLight", &spotLight);
        ImGui::ColorEdit3("Background Color", (float *)&bgColor); // Edit 3 floats representing a color
        ImGui::ColorEdit3("Floor Color", (float *)&floorColor);   // Edit 3 floats representing a color
        ImGui::InputFloat3("Model Position", modelPos);
        ImGui::InputFloat3("Model Scale", modelScale);
        ImGui::InputFloat3("dirLight Direction", lightDir);
        ImGui::InputFloat3("pointLight Position", lightPos);
        ImGui::InputInt("PointLight Shadow Type(0: hard 1: PCF 2: PCSS)", &shadowType);
        if(shadowType == 3) shadowType = 0;
        if(shadowType == -1) shadowType = 2;

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Camera Position: x: %.1f y: %.1f z: %.1f", camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        ImGui::End();
        ImGui::Render();

        // per-frame time logic 确保在所有硬件上移动速度都一样
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window); //输入控制
        // lightPos[0] = static_cast<float>(1 * sin(glfwGetTime()));
        // lightPos[1] = 3.0f;
        // lightPos[2] = static_cast<float>(1 * cos(glfwGetTime()));

        //渲染指令
        glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // ConfigureShaderAndMatrices();
        float near_plane = 0.1f, far_plane = 10.0f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);                                       //投影矩阵间接决定可视区域的范围，以及哪些东西不会被裁切，你需要保证投影视锥（frustum）的大小，以包含打算在深度贴图中包含的物体。当物体和片段不在深度贴图中时，它们就不会产生阴影
        glm::mat4 lightView = glm::lookAt(glm::vec3(-lightDir[0], -lightDir[1], -lightDir[2]), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0)); // 第一个参数为lightdirection
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
        GLfloat near = 0.1f;
        GLfloat far = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(glm::vec3(lightPos[0], lightPos[1], lightPos[2]), glm::vec3(lightPos[0], lightPos[1], lightPos[2]) + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(glm::vec3(lightPos[0], lightPos[1], lightPos[2]), glm::vec3(lightPos[0], lightPos[1], lightPos[2]) + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(glm::vec3(lightPos[0], lightPos[1], lightPos[2]), glm::vec3(lightPos[0], lightPos[1], lightPos[2]) + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(glm::vec3(lightPos[0], lightPos[1], lightPos[2]), glm::vec3(lightPos[0], lightPos[1], lightPos[2]) + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(glm::vec3(lightPos[0], lightPos[1], lightPos[2]), glm::vec3(lightPos[0], lightPos[1], lightPos[2]) + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(glm::vec3(lightPos[0], lightPos[1], lightPos[2]), glm::vec3(lightPos[0], lightPos[1], lightPos[2]) + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

        glm::mat4 shadowmodel = glm::mat4(1.0f);
        shadowmodel = glm::translate(shadowmodel, glm::vec3(modelPos[0], modelPos[1], modelPos[2]));
        shadowmodel = glm::scale(shadowmodel, glm::vec3(modelScale[0], modelScale[1], modelScale[2]));

        // render scene from light's point of view
        depthShader.use();
        depthShader.set_uniform("lightSpaceMatrix", 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        depthShader.set_uniform("model", 1, GL_FALSE, glm::value_ptr(shadowmodel));

        pointDepthShader.use();
        pointDepthShader.set_uniform("lightPos", lightPos[0], lightPos[1], lightPos[2]);
        pointDepthShader.set_uniform("far_plane", far);
        for (int i = 0; i < 6; i++)
            pointDepthShader.set_uniform("shadowMatrices[" + std::to_string(i) + "]", 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
        pointDepthShader.set_uniform("model", 1, GL_FALSE, glm::value_ptr(shadowmodel));

        shadow.RenderShadow(model_, floor_, depthShader, pointDepthShader);


        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //--
        shader_.use();
        shader_.set_uniform("viewPos", camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        shader_.set_uniform("material.shininess", 64.0f);
        shader_.set_uniform("isTexture", 1);
        shader_.set_uniform("shadowType", shadowType);
        // directional light
        shader_.set_uniform("dirLight.enable", dirLight);
        shader_.set_uniform("dirLight.direction", lightDir[0], lightDir[1], lightDir[2]);
        shader_.set_uniform("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        shader_.set_uniform("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader_.set_uniform("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        shader_.set_uniform("pointLights[0].enable", pointLight);
        // shader_.set_uniform("pointLights[0].position", static_cast<float>(2 * sin(glfwGetTime())), 2.0f, static_cast<float>(2 * cos(glfwGetTime())));
        shader_.set_uniform("pointLights[0].position", lightPos[0], lightPos[1], lightPos[2]);
        shader_.set_uniform("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        shader_.set_uniform("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        shader_.set_uniform("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        shader_.set_uniform("pointLights[0].constant", 1.0f);
        shader_.set_uniform("pointLights[0].linear", 0.09f);
        shader_.set_uniform("pointLights[0].quadratic", 0.032f);
        // spotLight
        shader_.set_uniform("spotLight.enable", spotLight);
        shader_.set_uniform("spotLight.position", camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        shader_.set_uniform("spotLight.direction", camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
        shader_.set_uniform("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        shader_.set_uniform("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        shader_.set_uniform("spotLight.specular", 1.0f, 1.0f, 1.0f);
        shader_.set_uniform("spotLight.constant", 1.0f);
        shader_.set_uniform("spotLight.linear", 0.09f);
        shader_.set_uniform("spotLight.quadratic", 0.032f);
        shader_.set_uniform("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        shader_.set_uniform("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // mvp
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(modelPos[0], modelPos[1], modelPos[2]));   // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(modelScale[0], modelScale[1], modelScale[2])); // it's a bit too big for our scene, so scale it down

        shader_.set_uniform("view", 1, GL_FALSE, glm::value_ptr(view));
        shader_.set_uniform("projection", 1, GL_FALSE, glm::value_ptr(projection));
        shader_.set_uniform("model", 1, GL_FALSE, glm::value_ptr(model));
        shader_.set_uniform("lightSpaceMatrix", 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        shader_.set_uniform("far_plane", far);
        model_.Draw(shader_, 0);

        // floor
        model = glm::mat4(1.0f);
        // model = glm::scale(model, glm::vec3(1.0/6, 1.0/6, 1.0/6));
        shader_.set_uniform("model", 1, GL_FALSE, glm::value_ptr(model));
        shader_.set_uniform("isTexture", 0);
        shader_.set_uniform("material.shininess", 64.0f);
        shader_.set_uniform("selfColor", floorColor.x, floorColor.y, floorColor.z);
        shader_.set_uniform("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
        shader_.set_uniform("dirLight.specular", 1.0f, 1.0f, 1.0f);
        floor_.Draw(shader_, 0);

        // light
        lightShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
        model = glm::scale(model, glm::vec3(0.1f)); // a smaller cube
        lightShader.set_uniform("model", 1, GL_FALSE, glm::value_ptr(model));
        lightShader.set_uniform("view", 1, GL_FALSE, glm::value_ptr(view));
        lightShader.set_uniform("projection", 1, GL_FALSE, glm::value_ptr(projection));
        if (pointLight)
            light_.Draw();
        if (dirLight)
        {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(-lightDir[0], -lightDir[1], -lightDir[2]));

            lightShader.set_uniform("model", 1, GL_FALSE, glm::value_ptr(model));
            dirlight_.Draw();
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    //释放/删除之前的分配的所有资源
    glfwTerminate();
    return;
}