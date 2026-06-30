#include "Id.hpp"
#include <NameManager.hpp>
#include <Scene.h>
#include <cassert>

NameManager::NameManager(Scene* scene) : m_scene(scene), m_map() {
    assert(m_scene != nullptr);
    m_data.currentCount = 0;
}

NameManager::~NameManager() {}

void NameManager::create(Id::EntityId entityId, const std::string& name) {
    assert(!hasComponent(entityId));

    Id::idType index = Id::indexOf(entityId);
    if (index >= m_map.size())
        m_map.resize(index + 1, Id::invalidIndex);

    Id::ComponentIndex compIndex = m_data.currentCount;

    m_data.entity.push_back(entityId);
    m_data.name.push_back(name);

    m_map[index] = compIndex;
    m_data.currentCount++;
}

void NameManager::destroy(Id::EntityId entityId) {
    Id::ComponentIndex compIndex = getComponentIndex(entityId);
    Id::ComponentIndex lastIndex = m_data.currentCount - 1;

    if (compIndex != lastIndex) {
        // Swap-and-Pop
        m_data.entity[compIndex] = m_data.entity[lastIndex];
        m_data.name[compIndex] = std::move(m_data.name[lastIndex]);

        m_map[Id::indexOf(m_data.entity[compIndex])] = compIndex;
    }

    m_data.entity.pop_back();
    m_data.name.pop_back();

    m_map[Id::indexOf(entityId)] = Id::invalidIndex;
    m_data.currentCount--;
}

void NameManager::setName(Id::EntityId entityId, const std::string& name) {
    m_data.name[getComponentIndex(entityId)] = name;
}

const std::string& NameManager::getName(Id::EntityId entityId) const {
    return m_data.name[getComponentIndex(entityId)];
}

Id::EntityId NameManager::findEntityByName(const std::string& name) const {
    for (Id::ComponentIndex i = 0; i < m_data.currentCount; ++i) {
        if (m_data.name[i] == name) {
            return m_data.entity[i];
        }
    }
    return Id::invalidId;
}
