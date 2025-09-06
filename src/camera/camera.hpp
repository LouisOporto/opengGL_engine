#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float FOV = 45.0f;
const float PITCH = 0.0f;
const float YAW = -90.0f;

const float SENSTIVITY = 0.05f;
const float VELOCITY = 0.025f;

enum DIRECTION {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

class Camera {
    public:
        Camera(int scr_w, int scr_h, glm::vec3 position = {0.0f, 0.0f, 3.0f}, glm::vec3 front = {0.0f, 0.0f, -1.0f}, glm::vec3 up = {0.0, 1.0, 0.0}): m_fov(FOV), m_pitch(PITCH), m_yaw(YAW) {
            m_front = front;
            m_pos = position;
            m_worldUp = up;
            SCR_W = scr_w;
            SCR_H = scr_h;
            updateCameraVectors();
        }

        void handleKeyInput(DIRECTION dir, float deltaTime);
        void handleMouseInput(float xoffset, float yoffset, bool constrainPitch = true);
        void handleScrollInput(float yoffset);

        glm::mat4 getLookAt() { return glm::lookAt(m_pos, m_pos + m_front, m_up); }
        glm::mat4 getPerspective() { return glm::perspective(glm::radians(m_fov), SCR_W / (float)SCR_H, 0.1f, 100.0f); }
        glm::vec3 getPos() { return m_pos; }
        glm::vec3 getFront() { return m_front; }

        void setWidth(int value) { SCR_W = value; }
        void setHeight(int value) { SCR_H = value; }
    private:
        void updateCameraVectors();

        glm::vec3 m_front;
        glm::vec3 m_pos;
        glm::vec3 m_right;
        glm::vec3 m_up;
        
        int SCR_W;
        int SCR_H;

        glm::vec3 m_worldUp;
        float m_fov;
        float m_pitch;
        float m_yaw;
};

#endif // _CAMERA_H_