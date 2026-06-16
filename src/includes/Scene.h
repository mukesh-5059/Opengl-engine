#pragma once

#include <memory>
#include <Camera.h>

class Model;

class Scene {
    private:
        std::shared_ptr<Model> m_testModel;
        Camera* m_camera;

    public:
        Scene();
        ~Scene();

        void update(float deltaTime, GLFWwindow* window);
        void render(int width, int height);
        void onGui();
};
