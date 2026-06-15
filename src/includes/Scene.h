#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

class ShaderProgram;
class Material;
class Texture;
class Mesh;
class Camera;

class Scene {
    private:
        std::shared_ptr<ShaderProgram> m_testShader;
        Material* m_testMaterial;
        std::shared_ptr<Mesh> m_testMesh;
        Camera* m_camera;
        std::shared_ptr<Texture> m_testTexture;
        std::shared_ptr<Texture> m_testTexture1;

    public:
        Scene();
        ~Scene();

        void update(float deltaTime, GLFWwindow* window);
        void render(int width, int height);
        void onGui();
};
