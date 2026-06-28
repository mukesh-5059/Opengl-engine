#pragma once

#include <memory>
#include <vector>
#include <Camera.h>

class Entity;
class Renderer;

class Scene {
    private:
        std::vector<std::shared_ptr<Entity>> m_entities;
        Camera* m_camera;
        int m_selectedEntityIndex = -1;

    public:
        Scene();
        ~Scene();

        void update(float deltaTime, GLFWwindow* window);
        void render(Renderer* renderer, int width, int height);
        void onHierarchyGui();
        void onInspectorGui();
};
