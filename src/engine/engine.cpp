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
        fprintf(stderr, "Failed to create window\n");
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
        fprintf(stderr, "Failed to init glew\n");
        return false;
    }
    
    // OPENGL setup
    glGenVertexArrays(1, &m_objectVAO);
    glGenVertexArrays(1, &m_lightVAO);
    glGenBuffers(1, &m_VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
    
    // glBindVertexArray(m_objectVAO);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
    // glEnableVertexAttribArray(0);
    
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
    // glEnableVertexAttribArray(1);
    
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
    // glEnableVertexAttribArray(2);
    
    glBindVertexArray(m_lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
    glEnableVertexAttribArray(0);
    
    glEnable(GL_DEPTH_TEST);
    
    // Textures setup
    // stbi_set_flip_vertically_on_load(true);
    // m_texture0 = ImageLoader::getInstance()->loadImage("box.png", "images");
    // m_texture1 = ImageLoader::getInstance()->loadImage("box_specular.png", "images");
   
    
    // m_objModel = new Model("./images/backpack/backpack.obj");
    m_objModel = new Model("./images/bunny/bunnygirl.obj");


    // Shader setup
    if (!m_objShader.initShader("src/shader/object.vert", "src/shader/object.frag")) {
        return false;
    }
    
    if (!m_lightShader.initShader("src/shader/light.vert", "src/shader/light.frag")) {
        return false;
    }
    
    m_objShader.use();
    // m_objShader.setInt("material.diffuse", 0);
    // m_objShader.setInt("material.specular", 1);
    m_objShader.setFloat("material.shininess", 32.0f);
    
    // Camera setup
    m_camera = new Camera(m_SCR_W, m_SCR_H);
    m_projection = glm::perspective(glm::radians(45.0f), (float)SCR_W / SCR_H, 0.1f, 100.0f);
    m_view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    setFirstMouse(true);
    m_lightOn = false;
    float temp = m_timer.getElapsed(); // Not used

    Vertex vertex;
    vertex.position = glm::vec3(1.0f);
    vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    vertex.texCoords = glm::vec2(0.0f, 0.0f);

    return m_running = true;
}

void Engine::event() {
    float dt = m_timer.getElapsed();
    if (glfwWindowShouldClose(m_window)) quit();
    // camera handle keyboard input
    handleKeyInput(dt);
    glfwPollEvents();
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
    m_projection = getCamera()->getPerspective();
    m_view = getCamera()->getLookAt();

    glm::vec3 directionVector = {0.0f, -20.3f, 4.3f};

    m_lightShader.use();
    m_lightShader.setMat4("projection", m_projection);
    m_lightShader.setMat4("view", m_view);

    // glm::vec3 lightColor = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 lightColor = glm::vec3(1.0f);

    m_objShader.use();
    m_objShader.setMat4("projection", m_projection);
    m_objShader.setMat4("view", m_view);
    m_objShader.setVec3("viewPos", getCamera()->getPos());
    
    m_objShader.setDirLight("dirLight", directionVector, lightColor * AMB, lightColor * DIF, lightColor * SPE);
    m_objShader.setInt("numPointLights", sizeof(LIGHTPOSITIONS) / sizeof(glm::vec3));
    for (int iter = 0; iter < sizeof(LIGHTPOSITIONS) / sizeof(glm::vec3); iter++) {
        glm::vec3 color = {iter == 0 ? 1.0f : 0.7f, iter == 1 ? 1.0f : 0.7f, iter == 2 ? 1.0f : 0.7f};
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        m_objShader.setPointLight("pointLights[" + std::to_string(iter) + ']', LIGHTPOSITIONS[iter], color * AMB, color * DIF, color * SPE, CONSTANT, LINEAR, QUADRATIC);
    }
    glm::vec3 spotlightColor = {1.0f, 1.0f, 0.3f};
    m_objShader.setBool("spotLightOn", m_lightOn);
    m_objShader.setSpotLight("spotLight", getCamera()->getPos(), getCamera()->getFront(), spotlightColor * AMB, spotlightColor * DIF, spotlightColor * SPE, cos(glm::radians(12.5f)), cos(glm::radians(17.5f)), CONSTANT, LINEAR, QUADRATIC);
    

}
void Engine::render() {
    glm::vec4 background = {0.1f, 0.1f, 0.1f, 1.0f};
    glClearColor(background.x, background.y, background.z, background.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Cube
    m_objShader.use();

    m_model = glm::mat4(1.0f);
    m_model = glm::translate(m_model, glm::vec3(0.0f, 0.0f, 0.0f));
    m_model = glm::rotate(m_model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 inverseModel = glm::inverse(m_model);

    m_objShader.setMat4("model", m_model);
    m_objShader.setMat4("inverseModel", inverseModel);

    m_objModel->draw(m_objShader);
    
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, m_texture0);
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, m_texture1);
    
    // for (int iter = 0; iter < sizeof(OBJECTPOSITIONS) / sizeof(glm::vec3); iter++) {
    //     m_model = glm::mat4(1.0f);
    //     m_model = glm::translate(m_model, OBJECTPOSITIONS[iter]);
    //     m_model = glm::rotate(m_model, glm::radians(iter * 15.f), glm::vec3(0.1f, 0.5f, 0.4f));
    //     glm::mat4 inverseModel = glm::inverse(m_model);

    //     m_objShader.setMat4("model", m_model);
    //     m_objShader.setMat4("inverseModel", inverseModel);

    //     glBindVertexArray(m_objectVAO);
    //     glDrawArrays(GL_TRIANGLES, 0, 36);
    // }

    // Light
    m_lightShader.use();

    for (int iter = 0; iter < sizeof(LIGHTPOSITIONS) / sizeof(glm::vec3); iter++) {
        m_model = glm::mat4(1.0f);
        m_model = glm::translate(m_model, LIGHTPOSITIONS[iter]);
        m_model = glm::scale(m_model, glm::vec3(0.6f, 0.6f, 0.6f));

        m_lightShader.setMat4("model", m_model);
        m_lightShader.setVec3("lightColor", glm::vec3(iter == 0 ? 1.0f : 0.0f, iter == 1 ? 1.0f : 0.0f, iter == 2 ? 1.0f : 0.0f));
        m_lightShader.setVec3("lightColor", glm::vec3(1.f, 1.0f, 1.f));

        glBindVertexArray(m_lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(m_window);
}

// Callback functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) { glfwSetWindowShouldClose(window, GL_TRUE); }
    if (key == GLFW_KEY_F && action == GLFW_PRESS) { Engine::getInstance()->toggleLight(); }
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

    // Handle in camera
    Engine::getInstance()->getCamera()->handleMouseInput(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Handle in camera
    Engine::getInstance()->getCamera()->handleScrollInput((float)yoffset);
}