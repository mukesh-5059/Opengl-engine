#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    private:
        glm::vec3 m_pos;
        glm::vec3 m_front;
        glm::vec3 m_up;
        glm::vec3 m_right;
        glm::vec3 m_worldUp;

        float m_yaw;
        float m_pitch;
        float m_movementSpeed;
        float m_mouseSensitivity;
        float m_fov;

        bool m_firstClick;

        void updateCameraVectors();

    public:
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
        ~Camera();

        void captureInput(GLFWwindow* window, float dt);
        void onGui();

        glm::mat4 getViewMatrix() const;
        glm::mat4 getProjectionMatrix(int width, int height) const;

        void setRotation(float yaw, float pitch);
        
        glm::vec3 getPosition() const { return m_pos; }
        float getFOV() const { return m_fov; }
};
