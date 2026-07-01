#pragma once
#include "Id.h"
#include <glm/glm.hpp>
#include <cassert>
#include <unordered_map>

class Scene;

class CameraManager {
    private:
        struct DataInstance {
            Id::ComponentIndex currentCount = 0;
            Id::ComponentIndex allocated = 0;
            void* buffer = nullptr;

            Id::EntityId* entity = nullptr;
            float* fov = nullptr;
            float* nearPlane = nullptr;
            float* farPlane = nullptr;
            bool* isOrthographic = nullptr;
            float* orthoSize = nullptr;
            glm::mat4* viewMatrix = nullptr;
            glm::mat4* projectionMatrix = nullptr;
        };

        std::unordered_map<Id::EntityId, Id::ComponentIndex> m_map;
        DataInstance m_data;
        Scene* m_scene = nullptr;

    public:
        CameraManager(Scene* scene);
        ~CameraManager();

        bool hasComponent(Id::EntityId entityId) const {
            return m_map.find(entityId) != m_map.end();
        }

        Id::ComponentIndex getComponentIndex(Id::EntityId entityId) const {
            assert(hasComponent(entityId));
            return m_map.at(entityId);
        }

        void allocate(Id::ComponentIndex count);
        void create(Id::EntityId entityId);
        void destroy(Id::EntityId entityId);

        void setFOV(Id::EntityId entityId, float fov);
        float getFOV(Id::EntityId entityId) const;

        void setNearPlane(Id::EntityId entityId, float nearPlane);
        float getNearPlane(Id::EntityId entityId) const;

        void setFarPlane(Id::EntityId entityId, float farPlane);
        float getFarPlane(Id::EntityId entityId) const;

        void setOrthographic(Id::EntityId entityId, bool isOrthographic);
        bool isOrthographic(Id::EntityId entityId) const;

        void setOrthoSize(Id::EntityId entityId, float size);
        float getOrthoSize(Id::EntityId entityId) const;

        glm::mat4 getViewMatrix(Id::EntityId entityId) const;
        glm::mat4 getProjectionMatrix(Id::EntityId entityId) const;

        void onInspectorGui(Id::EntityId entityId);

        void update(int viewportWidth, int viewportHeight);

        Id::ComponentIndex getCameraCount() const { return m_data.currentCount; }
        Id::EntityId getCameraEntity(Id::ComponentIndex index) const {
            assert(index < m_data.currentCount);
            return m_data.entity[index];
        }
};
