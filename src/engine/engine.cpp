#include "engine.hpp"

Engine* Engine::m_instance = nullptr;

bool Engine::init(int argc, char* argv[]) {

    if (!glfwInit()) {
        fprintf(stderr, "Failed to start glfw\n");
        return false;
    }

    glfwWindowHint(GL_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = NULL;
    m_SCR_W = SCR_W;
    m_SCR_H = SCR_H;
    m_window = glfwCreateWindow(m_SCR_W, m_SCR_H, "Window", NULL, NULL);
    if (!m_window) {
        Logger::Error("Failed to create window");
        return false;
    }
    
    glfwMakeContextCurrent(m_window);
    glfwSetKeyCallback(m_window, key_callback);
    glfwSetFramebufferSizeCallback(m_window, frame_callback);
    glfwSetScrollCallback(m_window, scroll_callback);
    glfwSetCursorPosCallback(m_window, mouse_callback);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        Logger::Error("Failed to init glew");
        return false;
    }
    
    // OPENGL setup
    if (!initOpenGLVariables()) {
        Logger::Error("Failed to load something from OpenGL!");
        return false;
    }
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // m_objModel = new Model("./images/backpack/backpack.obj");
    m_objModel = new Model("RESOURCES/images/bunny/bunnygirl.obj");
    
    // Textures setup
    stbi_set_flip_vertically_on_load(true);
    // m_texture0 = ImageLoader::getInstance()->loadImage("grass.png", "RESOURCES/images");
    // m_texture1 = ImageLoader::getInstance()->loadImage("blending_transparent_window.png", "RESOURCES/images");

    std::vector<std::string> faces {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg",
    };

    stbi_set_flip_vertically_on_load(false);
    m_cubemapTexture = ImageLoader::getInstance()->loadCubemap(faces, "RESOURCES/images/skybox");

    // Shader setup
    if (!m_objShader.initShader("RESOURCES/shaders/object.vert", "RESOURCES/shaders/object.frag")) {
        return false;
    }
    
    if (!m_lightShader.initShader("RESOURCES/shaders/light.vert", "RESOURCES/shaders/light.frag")) {
        return false;
    }

    if (!m_screenShader.initShader("RESOURCES/shaders/screen.vert", "RESOURCES/shaders/screen.frag")) {
        return false;
    }

    if (!m_skyboxShader.initShader("RESOURCES/shaders/skybox.vert", "RESOURCES/shaders/skybox.frag")) {
        return false;
    }

    if (!m_cubeShader.initShader("RESOURCES/shaders/cube.vert", "RESOURCES/shaders/cube.frag")) {
        return false;
    }
    
    m_objShader.use();
    m_objShader.setFloat("material.shininess", 32.0f);

    // m_screenShader.use();
    // m_screenShader.setInt("screenTexture", 0);
    
    // Camera setup
    m_camera = new Camera(m_SCR_W, m_SCR_H);
    m_projection = glm::perspective(glm::radians(45.0f), (float)SCR_W / SCR_H, 0.1f, 100.0f);
    m_view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    setFirstMouse(true);
    m_lightOn = false;
    m_NormalMapOn = true;
    m_mouseVisible = false;
    float temp = m_timer.getElapsed(); // Not used

    // Initialize imGUI context
    ImGui::GetVersion();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init();

    return m_running = true;
}

bool Engine::initOpenGLVariables() {
    glGenVertexArrays(1, &m_objectVAO);
    glGenVertexArrays(1, &m_lightVAO);
    glGenVertexArrays(1, &m_vegetationVAO);
    glGenVertexArrays(1, &m_skyboxVAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_skyboxVBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    // Object VAO
    {
        glBindVertexArray(m_objectVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
    }

    // Light VAO (Phyiscal light objects)
    {
        glBindVertexArray(m_lightVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
        glEnableVertexAttribArray(0);
    }

    // Vegtation VAO
    {
        glBindVertexArray(m_vegetationVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
        glEnableVertexAttribArray(2);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SKYBOXVERTICES), SKYBOXVERTICES, GL_STATIC_DRAW);

    // Skybox VAO
    {
        glBindVertexArray(m_skyboxVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    // QUAD VERTICES
    {
        glGenVertexArrays(1, &m_quadVAO);
        glGenBuffers(1, &m_quadVBO);
        glBindVertexArray(m_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(QUADVERTICES), &QUADVERTICES, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
        glEnableVertexAttribArray(1);
    }

    // Framebuffer setup
    {
        glGenTextures(1, &m_textureColorBuffer);
        glBindTexture(GL_TEXTURE_2D, m_textureColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_W, SCR_H, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureColorBuffer, 0);
        
        glGenRenderbuffers(1, &m_RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_W, SCR_H);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::Error("Framebuffer is not complete, cannot proceed!");
            return false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    return true;
}

void Engine::event() {
    glfwPollEvents();
    float dt = m_timer.getElapsed();
    if (glfwWindowShouldClose(m_window)) quit();
    // camera handle keyboard input
    handleKeyInput(dt);

    // Handle imGui GUI
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
}

void Engine::handleKeyInput(float deltaTime) {
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) getCamera()->handleKeyInput(FORWARD, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) getCamera()->handleKeyInput(BACKWARD, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) getCamera()->handleKeyInput(LEFT, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) getCamera()->handleKeyInput(RIGHT, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) getCamera()->handleKeyInput(UP, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) getCamera()->handleKeyInput(DOWN, deltaTime);
}

void Engine::update() {
    // General variables used by all shaders
    m_projection = getCamera()->getPerspective();
    m_view = getCamera()->getLookAt();

    glm::vec3 directionVector = {0.0f, -20.3f, 4.3f};
    glm::vec3 lightColor = glm::vec3(1.0f);

    // Light shader
    m_lightShader.use();
    m_lightShader.setMat4("projection", m_projection);
    m_lightShader.setMat4("view", m_view);

    // Model shader
    m_objShader.use();
    m_objShader.setMat4("projection", m_projection);
    m_objShader.setMat4("view", m_view);
    m_objShader.setVec3("viewPos", getCamera()->getPos());
    
    // Model Lighting (Phong Lighting)
    m_objShader.setDirLight("dirLight", directionVector, lightColor * AMB, lightColor * DIF, lightColor * SPE);
    m_objShader.setInt("numPointLights", LIGHTPOSITIONS.size());
    for (int iter = 0; iter < LIGHTPOSITIONS.size(); iter++) {
        glm::vec3 color = {iter == 0 ? 1.0f : 0.7f, iter == 1 ? 1.0f : 0.7f, iter == 2 ? 1.0f : 0.7f};
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        m_objShader.setPointLight("pointLights[" + std::to_string(iter) + ']', LIGHTPOSITIONS[iter], color * AMB, color * DIF, color * SPE, CONSTANT, LINEAR, QUADRATIC);
    }
    glm::vec3 spotlightColor = {1.0f, 1.0f, 0.3f};
    m_objShader.setBool("spotLightOn", m_lightOn);
    m_objShader.setSpotLight("spotLight", getCamera()->getPos(), getCamera()->getFront(), spotlightColor * AMB, spotlightColor * DIF, spotlightColor * SPE, cos(glm::radians(12.5f)), cos(glm::radians(17.5f)), CONSTANT, LINEAR, QUADRATIC);
    m_objShader.setBool("NormalOn", m_NormalMapOn);

    // Skybox Shader
    m_skyboxShader.use();
    m_skyboxShader.setMat4("projection", m_projection);
    m_skyboxShader.setMat4("view", glm::mat4(glm::mat3(m_view)));

    // Cube Shader
    m_cubeShader.use();
    m_cubeShader.setMat4("projection", m_projection);
    m_cubeShader.setMat4("view", m_view);
    m_cubeShader.setVec3("cameraPos", getCamera()->getPos());
}

void Engine::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glm::vec4 background = {0.1f, 0.1f, 0.1f, 1.0f};
    glEnable(GL_DEPTH_TEST);
    glClearColor(background.x, background.y, background.z, background.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    // Model
    m_objShader.use();
    
    m_model = glm::mat4(1.0f);
    m_model = glm::translate(m_model, glm::vec3(0.0f, 0.0f, 0.0f));
    m_model = glm::rotate(m_model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 inverseModel = glm::inverse(m_model);
    
    m_objShader.setMat4("model", m_model);
    m_objShader.setMat4("inverseModel", inverseModel);
    
    // m_objModel->draw(m_objShader);

    // Reflecting model
    m_cubeShader.use();
    
    m_model = glm::mat4(1.0f);
    m_model = glm::translate(m_model, glm::vec3(0.0f, 0.0f, 0.0f));
    m_model = glm::rotate(m_model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    inverseModel = glm::inverse(m_model);
    
    m_cubeShader.setMat4("model", m_model);
    m_cubeShader.setMat4("inverseModel", inverseModel);
    
    m_objModel->draw(m_cubeShader);
    
    // Reflecting cubes
    m_cubeShader.use();
    for (int iter = 0; iter < OBJECTPOSITIONS.size(); iter++) {
        // Logger::Log("Rendering cube: #%d", iter);
        m_model = glm::mat4(1.0f);
        m_model = glm::translate(m_model, OBJECTPOSITIONS[iter]);
        m_model = glm::rotate(m_model, glm::radians(iter * 15.f), glm::vec3(0.1f, 0.5f, 0.4f));
        glm::mat4 inverseModel = glm::inverse(m_model);
        
        m_cubeShader.setMat4("model", m_model);
        m_cubeShader.setMat4("inverseModel", inverseModel);
        
        glBindVertexArray(m_objectVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    
    // Light
    m_lightShader.use();
    
    for (int iter = 0; iter < LIGHTPOSITIONS.size(); iter++) {
        m_model = glm::mat4(1.0f);
        m_model = glm::translate(m_model, LIGHTPOSITIONS[iter]);
        m_model = glm::scale(m_model, glm::vec3(0.6f, 0.6f, 0.6f));
        
        m_lightShader.setMat4("model", m_model);
        // m_lightShader.setVec3("lightColor", glm::vec3(iter == 0 ? 1.0f : 0.0f, iter == 1 ? 1.0f : 0.0f, iter == 2 ? 1.0f : 0.0f));
        m_lightShader.setVec3("lightColor", glm::vec3(1.f, 1.0f, 1.f));
        
        glBindVertexArray(m_lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    
    glDepthFunc(GL_LEQUAL);
    // Skybox
    m_skyboxShader.use();
    glBindVertexArray(m_skyboxVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_screenShader.use();
    glBindVertexArray(m_quadVAO);
    glBindTexture(GL_TEXTURE_2D, m_textureColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    ImGui::Begin("Statistics");
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Average m/s: %f, Framerate: %.1f FPS", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    // Display imGui context
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(m_window);
}

void Engine::clean() {
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glDeleteVertexArrays(1, &m_objectVAO);
    glDeleteVertexArrays(1, &m_lightVAO);
    glDeleteVertexArrays(1, &m_vegetationVAO);
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_quadVBO);
    glDeleteRenderbuffers(1, &m_RBO);
    glDeleteFramebuffers(1, &m_FBO);
    glfwTerminate();
}

// Callback functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) { glfwSetWindowShouldClose(window, GL_TRUE); }
    if (key == GLFW_KEY_F && action == GLFW_PRESS) { Engine::getInstance()->toggleLight(); }
    if (key == GLFW_KEY_M && action == GLFW_PRESS) { Engine::getInstance()->toggleNormalMap(); }
    if (key == GLFW_KEY_N && action == GLFW_PRESS) { Engine::getInstance()->toggleMouse(); }
}

void frame_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    // Update width and height values;
    Engine::getInstance()->getCamera()->setWidth(width);
    Engine::getInstance()->getCamera()->setHeight(height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (Engine::getInstance()->getFirstMouse()) {
        Engine::getInstance()->setLastX(xpos);
        Engine::getInstance()->setLastY(ypos);
        Engine::getInstance()->setFirstMouse(false);
    }

    if (!Engine::getInstance()->isMouseVisible()) {
        float xoffset = (float)xpos - Engine::getInstance()->getLastX();
        float yoffset = Engine::getInstance()->getLastY() - (float)ypos;
    
        // printf("xoffset: %f, yoffset: %f\n", xoffset, yoffset);
        Engine::getInstance()->setLastX((float)xpos);
        Engine::getInstance()->setLastY((float)ypos);
    
        // Handle in camera
        Engine::getInstance()->getCamera()->handleMouseInput(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Handle in camera
    Engine::getInstance()->getCamera()->handleScrollInput((float)yoffset);
}