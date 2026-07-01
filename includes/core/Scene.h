#pragma once
#include <Camera.h>
#include <ecs/EntityManager.h>
#include <ecs/TransformManager.h>
#include <ecs/NameManager.h>
#include <ecs/CameraManager.h>
#include <memory>
#include <vector>

class Entity;
class Renderer;

class Scene {
    private:
        std::vector<std::shared_ptr<Entity>> m_entities;
        Camera* m_camera;

        EntityManager m_entityManager;
        TransformManager m_transformManager;
        NameManager m_nameManager;
        CameraManager m_cameraManager;
        Id::EntityId m_selectedEntityId = Id::invalidId;
        Id::EntityId m_mainCameraEntity = Id::invalidId;

        void drawHierarchyNode(Id::EntityId entityId);

    public:
        Scene();
        ~Scene();

        void update(float deltaTime, GLFWwindow* window);
        void render(Renderer* renderer, int width, int height);
        void onHierarchyGui();
        void onInspectorGui();

        EntityManager& getEntityManager() { return m_entityManager; }
        const EntityManager& getEntityManager() const { return m_entityManager; }

        TransformManager& getTransformManager() { return m_transformManager; }
        const TransformManager& getTransformManager() const { return m_transformManager; }

        NameManager& getNameManager() { return m_nameManager; }
        const NameManager& getNameManager() const { return m_nameManager; }

        CameraManager& getCameraManager() { return m_cameraManager; }
        const CameraManager& getCameraManager() const { return m_cameraManager; }
};
