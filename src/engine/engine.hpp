#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../imgui/imgui.h"
#include "../../imgui/backends/imgui_impl_opengl3.h"
#include "../../imgui/backends/imgui_impl_glfw.h"

#include "../timer/timer.hpp"
#include "../shader/shader.hpp"
#include "../camera/camera.hpp"
#include "../image_loader/image_loader.hpp"
#include "../mesh/mesh.hpp"
#include "../model/model.hpp"
#include "logger.hpp"

const int SCR_W = 1920;
const int SCR_H = 1080;

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
    
    -0.5, -0.5, -0.5,  0.0,  0.0, -1.0,  0.0,  0.0,
    -0.5,  0.5, -0.5,  0.0,  0.0, -1.0,  0.0,  1.0,
     0.5,  0.5, -0.5,  0.0,  0.0, -1.0,  1.0,  1.0,
     -0.5, -0.5, -0.5,  0.0,  0.0, -1.0,  0.0,  0.0,
     0.5,  0.5, -0.5,  0.0,  0.0, -1.0,  1.0,  1.0,
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

const float SKYBOXVERTICES[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

const float QUADVERTICES[] = {
    -1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
     1.0f,  -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f
};

const std::vector<glm::vec3> OBJECTPOSITIONS = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(3.2f, 0.3f, -5.0f),
    glm::vec3(-2.4f, -2.f, 2.0f),
    glm::vec3(0.0f, 3.0f, -3.0f),
};

const std::vector<glm::vec3> LIGHTPOSITIONS = {
    glm::vec3(3.0f, 12.0f, 2.5f),
    glm::vec3(-3.0f, 6.0f, 1.3f),
    glm::vec3(0.0f, -1.2f, 3.2f),
};

const std::vector<glm::vec3> VEGETATION = {
    glm::vec3(-1.5f, 0.0f, -0.48f),
    glm::vec3(1.5f, 0.0f, 0.51f),
    glm::vec3(0.0f, 0.0f, 0.7f),
    glm::vec3(-0.3f, 0.0f, -2.3f),
    glm::vec3(0.5f, 0.0f, -0.6f),
};

const float AMB = 0.1f; 
const float DIF = 0.7f;
const float SPE = 1.0f;

const float CONSTANT = 1.0f;
const float LINEAR = 0.09f;
const float QUADRATIC = 0.032f;

class Engine {
    public:
        static Engine* getInstance() { return m_instance = m_instance != nullptr ? m_instance : new Engine(); }

        bool init(int argc, char* argv[]);

        void update();
        void render();
        void event();

        void clean();
        inline bool isRunning() { return m_running; }
        
        void setLastX(const float& value) { m_lastX = value; }
        float getLastX() const { return m_lastX; }
        void setLastY(const float& value) { m_lastY = value; }
        float getLastY() const { return m_lastY; }
        void setFirstMouse(const bool& value) { m_firstMouse = value; }
        bool getFirstMouse() const { return m_firstMouse; } // Do we need this function and setter
        void toggleLight() { m_lightOn = !m_lightOn; } // Maybe remove
        void toggleNormalMap() { m_NormalMapOn = !m_NormalMapOn; } // Maybe remove
        void toggleMouse() {  // Re-edit
            m_mouseVisible = !m_mouseVisible; 
            glfwSetInputMode(m_window, GLFW_CURSOR, m_mouseVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        }
        
        Camera* getCamera() const { return m_camera; }

    private:
        bool initOpenGLVariables();
        void handleKeyInput(float deltaTime);
        inline void quit() { m_running = false; }

        
        Engine() {}
        static Engine* m_instance;
        GLFWwindow* m_window;

        // Streamline Model and Shader information (simplify if possible);
        std::map<int, Shader> m_shaders;
        std::map<std::string, int> m_shaderIndex;


        Model* m_objModel;
        Shader m_objShader;
        Shader m_lightShader;
        Shader m_screenShader;
        Shader m_skyboxShader;
        Shader m_cubeShader;
        Timer m_timer;
        bool m_running;

        // Dynamically create VBO, VAO, (intialize by hard coding and reuse by caching)
        std::map<std::string, unsigned int> m_VAOs;
        std::map<std::string, unsigned int> m_VBOs;
        // unsigned int m_FBO;
        // unsigned int m_RBO;

        unsigned int m_VBO, m_objectVAO, m_lightVAO, m_quadVAO, m_quadVBO, m_vegetationVAO, m_skyboxVAO, m_skyboxVBO, m_FBO, m_RBO;
        unsigned int m_texture0, m_texture1, m_textureColorBuffer, m_cubemapTexture;

        // World perspective        
        bool m_firstMouse;
        bool m_lightOn;
        bool m_NormalMapOn;
        bool m_mouseVisible;
        float m_lastX, m_lastY;

        // No need to change universal usage here
        Camera* m_camera;
        glm::mat4 m_projection;
        glm::mat4 m_view;
        glm::mat4 m_model; 

        int m_SCR_W, m_SCR_H; // Functions that change with screen size needs to update with this
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void frame_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#endif // _ENGINE_H_