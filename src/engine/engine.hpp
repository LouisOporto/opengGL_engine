#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../timer/timer.hpp"
#include "../shader/shader.hpp"
#include "../camera/camera.hpp"
#include "../image_loader/image_loader.hpp"

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
     0.5, -0.5, -0.5,  0.0, -1.0,  0.0,  1.0,  0.0,
     0.5, -0.5,  0.5,  0.0, -1.0,  0.0,  1.0,  1.0,
     0.5, -0.5,  0.5,  0.0, -1.0,  0.0,  1.0,  1.0,
    -0.5, -0.5,  0.5,  0.0, -1.0,  0.0,  0.0,  1.0,
    -0.5, -0.5, -0.5,  0.0, -1.0,  0.0,  0.0,  0.0,
};

const glm::vec3 OBJECTPOSITIONS[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),
};

const glm::vec3 LIGHTPOSITIONS[] = {
    glm::vec3(0.0f, 1.0f, 0.0f),
};

const glm::vec3 AMB = {0.1f, 0.1f, 0.1f};
const glm::vec3 DIF = {0.5f, 0.5f, 0.5f};
const glm::vec3 SPE = {1.0f, 1.0f, 1.0f};

const float CONSTANT = 1.0f;
const float LINEAR = 0.09f;
const float QUADRATIC = 0.032f;

class Engine {
    public:
        static Engine* getInstance() { return m_instance = m_instance != nullptr ? m_instance : new Engine(); }

        bool init(int argc, char* agrv[]);

        void update();
        void render();
        void event();

        inline void clean() { glfwTerminate(); }
        inline bool isRunning() { return m_running; }
        
        void setLastX(const float& value) { m_lastX = value; }
        float getLastX() const { return m_lastX; }
        void setLastY(const float& value) { m_lastY = value; }
        float getLastY() const { return m_lastY; }
        void setFirstMouse(const bool& value) { m_firstMouse = value; }
        bool getFirstMouse() const { return m_firstMouse; }
        Camera* getCamera() const { return m_camera; }
    private:
        void handleKeyInput(float deltaTime);
        inline void quit() { m_running = false; }

        
        Engine() {}
        static Engine* m_instance;
        GLFWwindow* m_window;
        Shader m_objShader;
        Shader m_lightShader;
        Timer m_timer;
        bool m_running;

        unsigned int m_VBO, m_objectVAO, m_lightVAO;
        unsigned int m_texture0, m_texture1;

        // World perspective        
        bool m_firstMouse;
        float m_lastX, m_lastY;

        Camera* m_camera;
        glm::mat4 m_projection;
        glm::mat4 m_view;
        glm::mat4 m_model; 
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void frame_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#endif // _ENGINE_H_