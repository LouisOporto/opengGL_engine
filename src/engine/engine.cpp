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
    m_window = glfwCreateWindow(SCR_W, SCR_H, "Window", NULL, NULL);
    if (!m_window) {
        fprintf(stderr, "Failed to create window\n");
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSetKeyCallback(m_window, key_callback);
    glfwSetFramebufferSizeCallback(m_window, frame_callback);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to init glew\n");
        return false;
    }
    
    // OPENGL setup
    glGenVertexArrays(1, &m_objectVAO);
    glGenVertexArrays(1, &m_lightVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glBindVertexArray(m_objectVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);
    
    glEnable(GL_DEPTH_TEST);

    // Textures setup

    // Shader setup
    if (!m_objShader.initShader("src/shader/object.vert", "src/shader/object.frag")) {
        return false;
    }

    return m_running = true;
}

void Engine::event() {
    if (glfwWindowShouldClose(m_window)) quit();

    glfwPollEvents();
}

void Engine::render() {
    glm::vec4 background = {0.1f, 0.1f, 0.2f, 1.0f};
    glClearColor(background.x, background.y, background.z, background.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_objShader.use();

    glBindVertexArray(m_objectVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(m_window);
}

// Callback functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) { 
        printf("Quitting\n");
        glfwSetWindowShouldClose(window, GL_TRUE); 
    }
}

void frame_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}