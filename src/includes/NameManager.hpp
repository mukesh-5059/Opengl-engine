#pragma once
#include "Id.hpp"
#include <string>
#include <vector>
#include <cassert>

class Scene;

class NameManager {
    private:
        Scene* m_scene = nullptr;

        struct DataInstance {
            Id::ComponentIndex currentCount = 0;
            std::vector<Id::EntityId> entity;
            std::vector<std::string> name;
        };

        std::vector<Id::ComponentIndex> m_map; // Entity Index -> Component Index
        DataInstance m_data;

    public:
        NameManager(Scene* scene);
        ~NameManager();

        bool hasComponent(Id::EntityId entityId) const {
            Id::idType idx = Id::indexOf(entityId);
            return idx < m_map.size() && m_map[idx] != Id::invalidIndex;
        }

        Id::ComponentIndex getComponentIndex(Id::EntityId entityId) const {
            assert(hasComponent(entityId));
            return m_map[Id::indexOf(entityId)];
        }

        void create(Id::EntityId entityId, const std::string& name);
        void destroy(Id::EntityId entityId);

        void setName(Id::EntityId entityId, const std::string& name);
        const std::string& getName(Id::EntityId entityId) const;

        void onInspectorGui(Id::EntityId entityId);

        Id::EntityId findEntityByName(const std::string& name) const;
};
