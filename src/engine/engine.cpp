#include "engine.hpp"

Engine* Engine::m_instance = nullptr;

bool Engine::init(int argc, char* argv[]) {
    if (!glfwInit()) {
        Logger::Error("Failed to start GLFW");
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
        Logger::Error("Failed to create GLFW window");
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
        Logger::Error("Failed to init GLEW");
        return false;
    }
    
    // OPENGL setup
    if (!initOpenGLVariables()) {
        Logger::Error("Failed to load something from OpenGL Variables!");
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
    m_objModel = new Model("RESOURCES/images/bunny/bunnygirl.obj"); // Not all models are loading correctly
    
    // Textures setup
    std::vector<std::string> faces {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg",
    };

    m_cubemapTexture = ImageLoader::getInstance()->loadCubemap(faces, "RESOURCES/images/skybox");

    // Shader setup
    if (!setupShaders()) {
        return false;
    }
    
    m_objShader.use();
    m_objShader.setFloat("material.shininess", 32.0f);

    // m_screenShader.use();
    // m_screenShader.setInt("screenTexture", 0);
    
    // Camera setup
    m_camera = new Camera(m_SCR_W, m_SCR_H);
    m_projection = glm::perspective(glm::radians(45.0f), (float)m_SCR_W / m_SCR_H, 0.1f, 100.0f);
    m_view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    setFirstMouse(true);
    m_lightOn = false;
    m_NormalMapOn = true;
    m_mouseVisible = false;
    m_screenRotate = false;
    toggleMouse();
    float temp = m_timer.getElapsed(); // Not used

    // Initialize imGUI context
    ImGui::GetVersion();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init();
    m_loadingBuffer[0] = '\0';
    m_eventBuffer[0] = '\0';
    m_pathBuffer[0] = '\0';
    m_playIndex = 0;

    if (!AudioEngine::getInstance()->init()) {
        Logger::Error("Failed to load FMOD");
        return false;
    }

    AudioEngine::getInstance()->loadBank("Master.strings", "RESOURCES/audio");
    AudioEngine::getInstance()->loadBank("Master", "RESOURCES/audio");
    AudioEngine::getInstance()->setActiveBank("Master");
    // AudioEngine::getInstance()->playTest("Master.bank");
    // AudioEngine::getInstance()->playByPath("event:/Music", "Radio");

    // AudioEngine::getInstance()->loadBank("Master", "RESOURCES/audio/Dispatch");
    // AudioEngine::getInstance()->loadBank("Ep106Music", "RESOURCES/audio/Dispatch");
    // Radio by Bershy is Ep106Music index 12
    // AudioEngine::getInstance()->playByIndex("Ep106Music", "Radio", 12);

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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_SCR_W, m_SCR_H, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureColorBuffer, 0);
        
        glGenRenderbuffers(1, &m_RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_SCR_W, m_SCR_H);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::Error("Framebuffer is not complete, cannot proceed!");
            return false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    return true;
}

bool Engine::setupShaders() {
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

    return true;
}

void Engine::event() {
    glfwPollEvents();
    if (glfwWindowShouldClose(m_window)) quit();
    float dt = m_timer.getElapsed();
    handleKeyInput(dt);

    // Handle imGui GUI
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void Engine::update() {
    AudioEngine::getInstance()->update();
    
    // General variables used by all shaders
    if (m_screenRotate) {
        getCamera()->setPos(glm::vec3(20.0f * glm::sin(glfwGetTime() * 0.1f) + 2.5f, 5.0f, 20.0f * glm::cos(glfwGetTime() * 0.1f)));
        getCamera()->setFront(glm::vec3(-20.0f * glm::sin(glfwGetTime() * 0.1f) + 2.5f, 0.0f, -20.0f * glm::cos(glfwGetTime() * 0.1f)));
        getCamera()->setUp(glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
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
    
    m_objModel->draw(m_objShader);

    // Reflecting model
    m_cubeShader.use();
    
    m_model = glm::mat4(1.0f);
    m_model = glm::translate(m_model, glm::vec3(10.0f, 0.0f, 0.0f));
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
    
    // Skybox (Final Texture should be interchangeable if possible)
    glDepthFunc(GL_LEQUAL);
    m_skyboxShader.use();
    glBindVertexArray(m_skyboxVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    // Framebuffer render to the simplified triangle faces
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_screenShader.use();
    glBindVertexArray(m_quadVAO);
    glBindTexture(GL_TEXTURE_2D, m_textureColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Handle ImGui a the end of render line
    renderImGuiInterface();

    // Display ImGui context
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(m_window);
}

void Engine::clean() {
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    // Clean up by oject basis or keep this way
    glDeleteVertexArrays(1, &m_objectVAO);
    glDeleteVertexArrays(1, &m_lightVAO);
    glDeleteVertexArrays(1, &m_vegetationVAO);
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_quadVBO);
    glDeleteRenderbuffers(1, &m_RBO);
    glDeleteFramebuffers(1, &m_FBO);

    glfwTerminate();
    AudioEngine::getInstance()->clean();
}

void Engine::handleKeyInput(float deltaTime) {
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) getCamera()->handleKeyInput(FORWARD, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) getCamera()->handleKeyInput(BACKWARD, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) getCamera()->handleKeyInput(LEFT, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) getCamera()->handleKeyInput(RIGHT, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) getCamera()->handleKeyInput(UP, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_C) != GLFW_PRESS) getCamera()->handleKeyInput(DOWN, deltaTime);
}

// Callback functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_C && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL) { Engine::getInstance()->copyFunction(); }
    // if (key == GLFW_KEY_C && action == GLFW_PRESS && mods != GLFW_MOD_CONTROL) {printf("Presing just C");}
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) { glfwSetWindowShouldClose(window, GL_TRUE); }
    if (key == GLFW_KEY_F && action == GLFW_PRESS) { Engine::getInstance()->toggleLight(); }
    if (key == GLFW_KEY_N && action == GLFW_PRESS) { Engine::getInstance()->toggleNormalMap(); }
    if (key == GLFW_KEY_M && action == GLFW_PRESS) { Engine::getInstance()->toggleMouse(); }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) { Engine::getInstance()->toggleRotate(); }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) { AudioEngine::getInstance()->setTimelinePosition(0.0); }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) { AudioEngine::getInstance()->setTimelinePosition(0.25); }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) { AudioEngine::getInstance()->setTimelinePosition(0.50); }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS) { AudioEngine::getInstance()->setTimelinePosition(0.75); }
    
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

    float xoffset = (float)xpos - Engine::getInstance()->getLastX();
    float yoffset = Engine::getInstance()->getLastY() - (float)ypos;
    
    // printf("xoffset: %f, yoffset: %f\n", xoffset, yoffset);
    Engine::getInstance()->setLastX((float)xpos);
    Engine::getInstance()->setLastY((float)ypos);
    
    if (!Engine::getInstance()->isMouseVisible() && !Engine::getInstance()->isScreenRotate()) {
        // Handle in camera
        Engine::getInstance()->getCamera()->handleMouseInput(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Handle in camera
    Engine::getInstance()->getCamera()->handleScrollInput((float)yoffset);
}

// ImGUI Functions
void Engine::renderImGuiInterface() {
    // ImGui::ShowDemoWindow();
    showappMenuBar();

    if (!ImGui::Begin("User Interface")) {
        ImGui::End();
        return;
    }
    else {
        showTools();
        showMusicPlayer("Radio");
        showFramerateStatistics();
        ImGui::End();
    }
}

void Engine::showappMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            ImGui::MenuItem("Main menu", NULL, false, false); // No "Shortcut key provided"
            ImGui::Separator();
            ImGui::MenuItem("Setttings");
            if (ImGui::MenuItem("Quit")) { quit(); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {}
            if (ImGui::MenuItem("Open")) {}
            if (ImGui::BeginMenu("Open Recent")) {
                ImGui::MenuItem("last.c"); // Does nothing since no if () around
                ImGui::MenuItem("previous.ini");
                if (ImGui::MenuItem("engine.hpp")) { printf("Gets last engine if it existed\n"); }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) { printf("Undo\n"); } // Match a shortcut key with callback key (if CTRL + Z) then relative
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {} // 1. False to just get to last bool 2. false to be not enabled (unless redo is available)
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X"));
            if (ImGui::MenuItem("Copy", "CTRL+C")) { copyFunction(); }
            if (ImGui::MenuItem("Paste", "CTRL+V"));
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Engine::showTools() {
    if (ImGui::CollapsingHeader("Tools")) {
    }
}

void Engine::showMusicPlayer(std::string name) {
    if (ImGui::CollapsingHeader("Music Player")) {
        ImGui::Text("Active Bank: %s, Active Event: %s", AudioEngine::getInstance()->getActiveBankName().c_str(), AudioEngine::getInstance()->getActiveEventName().c_str());
        if (AudioEngine::getInstance()->checkInstance(name)) {
            Event event = AudioEngine::getInstance()->getActiveEvent();
            ImGui::Text("Current Song: %s", event.name.c_str());
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            ImGui::ProgressBar(event.currentPos / (float)event.totalPos, ImVec2(0.0f, 20.0f), event.isPaused ? "Paused..." : "Playing...");
            ImGui::PopStyleColor(2);
            ImGui::Text("Current: %d:%02d Total: %d:%02d", event.currentPos / 60000, event.currentPos / 1000 % 60, event.totalPos / 60000, event.totalPos / 1000 % 60);
            ImGui::Text("Option to rewind, play, pause, stop, forward");
            if (ImGui::DragInt("Volume", &m_volume, (0.5F), 0, 100, "%d%")) AudioEngine::getInstance()->setSoundVolume(m_volume);
        }
        else {
            ImGui::Text("Current song:\nCurrent point in the song\nOptions to rewind, play, pause, stop, forward\n");
        }
        if (ImGui::TreeNode("Audio Engine Functions")) {
            ImGui::SeparatorText("Load & Unload Bank");
            ImGui::InputText("File Name", m_loadingBuffer, sizeof(m_loadingBuffer)); 
            if (ImGui::Button("Set Active Bank")) AudioEngine::getInstance()->setActiveBank(std::string(m_loadingBuffer));
            ImGui::SameLine(); if (ImGui::Button("Load")) AudioEngine::getInstance()->loadBank(std::string(m_loadingBuffer), "./resources/audio");
            ImGui::SameLine(); if (ImGui::Button("Unload")) AudioEngine::getInstance()->dropBank(std::string(m_loadingBuffer));

            ImGui::SeparatorText("Play Events");
            ImGui::InputText("Selected Event Name", m_eventBuffer, sizeof(m_eventBuffer));
            if (ImGui::Button("Set Active Event")) { AudioEngine::getInstance()->setActiveEvent(m_eventBuffer); }
            ImGui::SameLine(); if (ImGui::Button("Remove Event")) { AudioEngine::getInstance()->stop(); }
            ImGui::SameLine(); if (ImGui::Button("Release Event")) { AudioEngine::getInstance()->releaseInstance(); }
            ImGui::SameLine(); if (ImGui::Button("Pause Event")) { AudioEngine::getInstance()->pause(); }
            ImGui::SameLine(); if (ImGui::Button("Play Event")) { AudioEngine::getInstance()->resume(); }

            ImGui::SeparatorText("ByPath");
            ImGui::InputText("Path", m_pathBuffer, sizeof(m_pathBuffer));
            if (ImGui::Button("Play Path")) { AudioEngine::getInstance()->playByPath(m_eventBuffer, m_pathBuffer); }

            ImGui::SeparatorText("ByIndex");
            ImGui::InputInt("Index", &m_playIndex, 0);
            if (ImGui::Button("Play Index")) { AudioEngine::getInstance()->playByIndex(m_eventBuffer, m_playIndex); }

            ImGui::TreePop();
        }

    }
}

void Engine::showFramerateStatistics() {
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::CollapsingHeader("Rendering Statistics")) {
        if (ImGui::TreeNode("Input")) {
            if (ImGui::IsMousePosValid()) {
                ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            }
            else {
                ImGui::Text("Mouse pos: <INVALID>");
            }
            ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
            
            ImGui::TreePop();
        }
        ImGui::Text("Average m/s: %f, Framerate: %.1f FPS", 1000.0f / io.Framerate, io.Framerate);
    }
}