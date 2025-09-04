#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float FOV = 45.0f;
const float PITCH = 0.0f;
const float YAW = -90.0f;

const float SENSTIVITY = 0.025f;
const float VELOCITY = 0.05f;

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
        Camera(glm::vec3 position = {0.0f, 0.0f, 3.0f}, glm::vec3 front = {0.0f, 0.0f, -1.0f}, glm::vec3 up = {0.0, 1.0, 0.0}): m_fov(FOV), m_pitch(PITCH), m_yaw(YAW) {
            m_front = front;
            m_pos = position;
            m_worldUp = up;
            updateCameraVectors();
        }

        void handleKeyInput(DIRECTION dir, float deltaTime);
        void handleScrollInput(float yoffset);
        void handleMouseInput(float xoffset, float yoffset, bool constrainPitch = true);

        glm::mat4 getLookAt() { return glm::lookAt(m_pos, m_front, m_up); }
        glm::mat4 getPerspective() { return glm::perspective(glm::radians(m_fov), 800 / 600.f, 0.1f, 100.0f); }
        glm::vec3 getPos() { return m_pos; }
    private:
        void updateCameraVectors();

        glm::vec3 m_front;
        glm::vec3 m_pos;
        glm::vec3 m_right;
        glm::vec3 m_up;

        glm::vec3 m_worldUp;
        float m_fov;
        float m_pitch;
        float m_yaw;
};

#endif // _CAMERA_H_