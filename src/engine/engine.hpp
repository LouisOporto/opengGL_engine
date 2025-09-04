#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../timer/timer.hpp"
// #include "../camera/camera.hpp"
// #include "../shader/shader.hpp"
// #include "../image_loader/image_loader.hpp"

const int SCR_W = 800;
const int SCR_H = 600;

const float VERTICES[] = {
    -0.5,  0.5,  0.5,  0.0,  0.0,  1.0,  0.0,  1.0,
    -0.5, -0.5,  0.5,  0.0,  0.0,  1.0,  0.0,  0.0,
     0.5,  0.5,  0.5,  0.0,  0.0,  1.0,  1.0,  1.0,
     0.5,  0.5,  0.5,  0.0,  0.0,  1.0,  1.0,  1.0,
    -0.5, -0.5,  0.5,  0.0,  0.0,  1.0,  0.0,  0.0,
     0.5, -0.5,  0.5,  0.0,  0.0,  1.0,  1.0,  0.0,

     0.5,  0.5,  0.5,  1.0,  0.0,  0.0,  0.0,  1.0,
     0.5, -0.5,  0.5,  1.0,  0.0,  0.0,  0.0,  0.0,
     0.5,  0.5, -0.5,  1.0,  0.0,  0.0,  1.0,  1.0,
     0.5,  0.5, -0.5,  1.0,  0.0,  0.0,  1.0,  1.0,
     0.5, -0.5,  0.5,  1.0,  0.0,  0.0,  0.0,  0.0,
     0.5, -0.5, -0.5,  1.0,  0.0,  0.0,  1.0,  0.0,

    -0.5,  0.5, -0.5, -1.0,  0.0,  0.0,  0.0,  1.0,
    -0.5, -0.5, -0.5, -1.0,  0.0,  0.0,  0.0,  0.0,
    -0.5,  0.5,  0.5, -1.0,  0.0,  0.0,  1.0,  1.0,
    -0.5,  0.5,  0.5, -1.0,  0.0,  0.0,  1.0,  1.0,
    -0.5, -0.5, -0.5, -1.0,  0.0,  0.0,  0.0,  0.0,
    -0.5, -0.5,  0.5, -1.0,  0.0,  0.0,  1.0,  0.0,
    
    -0.5,  0.5, -0.5,  0.0,  0.0, -1.0,  0.0,  1.0,
    -0.5, -0.5, -0.5,  0.0,  0.0, -1.0,  0.0,  0.0,
     0.5,  0.5, -0.5,  0.0,  0.0, -1.0,  1.0,  1.0,
     0.5,  0.5, -0.5,  0.0,  0.0, -1.0,  1.0,  1.0,
    -0.5, -0.5, -0.5,  0.0,  0.0, -1.0,  0.0,  0.0,
     0.5, -0.5, -0.5,  0.0,  0.0, -1.0,  1.0,  0.0,
    
    -0.5,  0.5, -0.5,  0.0,  1.0,  0.0,  0.0,  1.0,
    -0.5,  0.5,  0.5,  0.0,  1.0,  0.0,  0.0,  0.0,
     0.5,  0.5, -0.5,  0.0,  1.0,  0.0,  1.0,  1.0,
     0.5,  0.5, -0.5,  0.0,  1.0,  0.0,  1.0,  1.0,
    -0.5,  0.5,  0.5,  0.0,  1.0,  0.0,  0.0,  0.0,
     0.5,  0.5,  0.5,  0.0,  1.0,  0.0,  1.0,  0.0,
    
    -0.5, -0.5, -0.5,  0.0, -1.0,  0.0,  0.0,  0.0,
    -0.5, -0.5,  0.5,  0.0, -1.0,  0.0,  0.0,  1.0,
     0.5, -0.5,  0.5,  0.0, -1.0,  0.0,  1.0,  1.0,
     0.5, -0.5,  0.5,  0.0, -1.0,  0.0,  1.0,  1.0,
    -0.5, -0.5,  0.5,  0.0, -1.0,  0.0,  0.0,  1.0,
     0.5, -0.5, -0.5,  0.0, -1.0,  0.0,  1.0,  0.0,
};

const glm::vec3 OBJECTPOSITIONS[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),
};

const glm::vec3 LIGHTPOSITIONS[] = {
    glm::vec3(0.0f, 3.0f, 0.0f),
};

class Engine {
    public:
        static Engine* getInstance() { return m_instance = m_instance != nullptr ? m_instance : new Engine(); }

        bool init(int argc, char* agrv[]);

        void update() {}
        void render();
        void event();

        inline void clean() { glfwTerminate(); }
        inline bool isRunning() { return m_running; }
        inline void quit() { m_running = false; }

    private:
        Engine() {}
        static Engine* m_instance;

        GLFWwindow* m_window;
        bool m_running;
        Timer timer;

        unsigned int VAO, objectBuffer, lightBuffer;
        // unsigned int texture0, texture1; 
        // Camera* camera;
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void frame_callback(GLFWwindow* window, int width, int height);

#endif // _ENGINE_H_