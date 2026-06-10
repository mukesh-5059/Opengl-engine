#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class ShaderProgram;
class Mesh;

class Application {
    private:
        ShaderProgram* m_testShader;
        Mesh* m_testMesh;

        GLFWwindow* m_window;
        std::string m_title;
        int m_width;
        int m_height;
        bool m_isRunning;
        bool m_vSync;
        int m_targetFps;
        float m_frameTimeHistory[100];
        int m_frameTimeIndex;

        bool init();
        void initImGui();
        void shutdownImGui();
        void processInput();
        void update(float deltaTime);
        void render();
        void onGui();

        // Static callback for window resizing
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    public:
        Application(const std::string& title, int width, int height);
        ~Application();

        void run();
        void stop();
};