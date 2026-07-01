#pragma once
#include "Id.h"
#include <glm/glm.hpp>
#include <vector>

class Scene;

class TransformManager {
    private:
        struct DataInstance {
            Id::ComponentIndex currentCount = 0;
            Id::ComponentIndex allocated = 0;
            void* buffer = nullptr;

            Id::EntityId* entity = nullptr;
            glm::vec3* pos = nullptr;
            glm::vec3* rot = nullptr;
            glm::vec3* scale = nullptr;
            glm::mat4* localMatrices = nullptr;
            glm::mat4* modelMatrices = nullptr;

            Id::ComponentIndex* parent = nullptr;
            Id::ComponentIndex* firstChild = nullptr;
            Id::ComponentIndex* nextSibling = nullptr;
            Id::ComponentIndex* prevSibling = nullptr;
        };

        std::vector<Id::ComponentIndex> m_map; // Entity Index -> Component Index
        DataInstance m_data;
        Scene* m_scene = nullptr;

        void rebuildLocalMatrix(Id::ComponentIndex index);
        void propagateTransform(Id::ComponentIndex index);

    public:
        TransformManager(Scene* scene);
        ~TransformManager();

        bool hasComponent(Id::EntityId entityId) const {
            Id::idType idx = Id::indexOf(entityId);
            return idx < m_map.size() && m_map[idx] != Id::invalidIndex;
        }

        Id::ComponentIndex getComponentIndex(Id::EntityId entityId) const {
            assert(hasComponent(entityId));
            return m_map[Id::indexOf(entityId)];
        }

        void allocate(Id::ComponentIndex count);
        void create(Id::EntityId entityId);
        void destroy(Id::EntityId entityId);
        void setParent(Id::EntityId childEntityId, Id::EntityId parentEntityId, bool keepWorld = true);

        void setPosition(Id::EntityId entityId, const glm::vec3& position);
        glm::vec3 getPosition(Id::EntityId entityId) const;

        void setRotation(Id::EntityId entityId, const glm::vec3& rotation);
        glm::vec3 getRotation(Id::EntityId entityId) const;

        void setScale(Id::EntityId entityId, const glm::vec3& scale);
        glm::vec3 getScale(Id::EntityId entityId) const;

        glm::mat4 getModelMatrix(Id::EntityId entityId) const;
        glm::mat4 getLocalMatrix(Id::EntityId entityId) const;

        Id::EntityId getParent(Id::EntityId entityId) const;
        Id::EntityId getFirstChild(Id::EntityId entityId) const;
        Id::EntityId getNextSibling(Id::EntityId entityId) const;

        void onInspectorGui(Id::EntityId entityId);

        void update();

        Id::ComponentIndex getEntityCount() const { return m_data.currentCount; }
        Id::EntityId getEntity(Id::ComponentIndex index) const {
            assert(index < m_data.currentCount);
            return m_data.entity[index];
        }
};