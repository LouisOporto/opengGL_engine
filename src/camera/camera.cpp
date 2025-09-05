#include "camera.hpp"

void Camera::handleKeyInput(DIRECTION dir, float deltaTime) {
    float speed = deltaTime * VELOCITY;
    switch (dir) {
        case FORWARD: m_pos += m_front * speed; break;
        case BACKWARD: m_pos -= m_front * speed; break;
        case LEFT: m_pos -= m_right * speed; break;
        case RIGHT: m_pos += m_right * speed; break;
        case UP: m_pos += m_up * speed; break;
        case DOWN: m_pos -= m_up * speed; break;
    }
}

void Camera::handleMouseInput(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= SENSTIVITY;
    yoffset *= SENSTIVITY;

    m_pitch += yoffset;
    m_yaw += xoffset;

    if (constrainPitch) {
        if (m_pitch > 89.0f) m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::handleScrollInput(float yoffset) {
    m_fov -= yoffset;
    if (m_fov < 0.1f) m_fov = 0.1f;
    if (m_fov > 45.0f) m_fov = 45.0f;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}