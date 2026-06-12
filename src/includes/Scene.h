#pragma once

#include <vector>
#include <Mesh.h>
#include <ShaderProgram.h>
#include <Material.h>
#include <Camera.h>

class Scene {
    private:
        ShaderProgram* m_testShader;
        Material* m_testMaterial;
        Mesh* m_testMesh;
        Camera* m_camera;

    public:
        Scene();
        ~Scene();

        void update(float deltaTime, GLFWwindow* window);
        void render(int width, int height);
        void onGui();
};
