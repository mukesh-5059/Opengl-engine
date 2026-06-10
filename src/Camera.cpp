#include <Camera.h>
#include <algorithm>
#include <imgui/imgui.h>
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_pos(position), m_worldUp(up), m_yaw(yaw), m_pitch(pitch), 
      m_movementSpeed(3.5f), m_mouseSensitivity(0.1f), m_fov(45.0f), m_firstClick(true) {
    std::cout << "[Camera] Initialized at pos: (" << m_pos.x << ", " << m_pos.y << ", " << m_pos.z << ")" << std::endl;
    updateCameraVectors();
}

Camera::~Camera() {
    std::cout << "[Camera] Destroyed" << std::endl;
}

void Camera::updateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    newFront.y = sin(glm::radians(m_pitch));
    newFront.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(newFront);
    
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::captureInput(GLFWwindow* window, float dt) {
    ImGuiIO& io = ImGui::GetIO();
    float velocity = m_movementSpeed * dt;

    if (!io.WantCaptureKeyboard) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) m_pos += m_front * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) m_pos -= m_front * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) m_pos -= m_right * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) m_pos += m_right * velocity;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) m_pos += m_worldUp * velocity;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) m_pos -= m_worldUp * velocity;
    }

    if (!io.WantCaptureMouse) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            int width, height;
            glfwGetWindowSize(window, &width, &height);

            if (m_firstClick) {
                glfwSetCursorPos(window, width / 2.0, height / 2.0);
                m_firstClick = false;
                return;
            }

            float xOffset = (float)(mouseX - (width / 2.0));
            float yOffset = (float)((height / 2.0) - mouseY);

            glfwSetCursorPos(window, width / 2.0, height / 2.0);

            m_yaw += xOffset * m_mouseSensitivity;
            m_pitch += yOffset * m_mouseSensitivity;

            m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

            updateCameraVectors();
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_firstClick = true;
        }
    }
}

void Camera::onGui() {
    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::DragFloat3("Position", (float*)&m_pos, 0.1f);
        ImGui::SliderFloat("FOV", &m_fov, 30.0f, 120.0f);
        ImGui::DragFloat("Speed", &m_movementSpeed, 0.1f, 0.1f, 10.0f);
        ImGui::DragFloat("Sensitivity", &m_mouseSensitivity, 0.01f, 0.01f, 1.0f);
        
        if (ImGui::Button("Reset View")) {
            m_pos = glm::vec3(0.0f, 0.0f, 3.0f);
            m_yaw = -90.0f;
            m_pitch = 0.0f;
            updateCameraVectors();
        }
    }
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_pos, m_pos + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix(int width, int height) const {
    float aspect = (float)width / (float)height;
    return glm::perspective(glm::radians(m_fov), aspect, 0.1f, 100.0f);
}

void Camera::setRotation(float yaw, float pitch) {
    m_yaw = yaw;
    m_pitch = pitch;
    updateCameraVectors();
}
