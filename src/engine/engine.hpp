#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// #include "../camera/camera.hpp"
// #include "../shader/shader.hpp"
// #include "../image_loader/image_loader.hpp"

const int SCR_W = 800;
const int SCR_H = 600;

class Engine {
    public:
        static Engine* getInstance() { return m_instance = m_instance != nullptr ? m_instance : new Engine(); }

        bool init(int argc, char* agrv[]);

        void update();
        void render();
        void event();

        bool isRunning() { return m_running; }

    private:
        Engine() {}
        static Engine* m_instance;

        GLFWwindow* m_window;
        bool m_running;
        // Camera* camera;
};