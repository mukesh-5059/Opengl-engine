#include "Id.hpp"
#include <TransformManager.hpp>
#include <Scene.h>
#include <imgui/imgui.h>
#include <cstdlib>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

TransformManager::TransformManager(Scene* scene) : m_scene(scene), m_map() {
    assert(m_scene != nullptr);
    m_data.currentCount = 0;
    m_data.allocated = 0;
    m_data.buffer = nullptr;
}

TransformManager::~TransformManager() {
    if (m_data.buffer) {
        free(m_data.buffer);
    }
}

void TransformManager::allocate(Id::ComponentIndex count) {
    assert(count >= m_data.currentCount);
    if (count <= m_data.allocated) return;

    // Round up allocation to a multiple of 16 to guarantee pointer alignment (16-byte alignment)
    count = (count + 15) & ~15;

    size_t bytes = count * (sizeof(Id::EntityId) * 1 + sizeof(glm::vec3) * 3 + sizeof(glm::mat4) * 2 + sizeof(Id::ComponentIndex) * 4);

    DataInstance newData;

    newData.buffer = malloc(bytes);
    newData.currentCount = m_data.currentCount;
    newData.allocated = count;

    newData.entity = (Id::EntityId*)(newData.buffer);
    newData.pos = (glm::vec3*)(newData.entity + count);
    newData.rot = newData.pos + count;
    newData.scale = newData.rot + count;
    newData.localMatrices = (glm::mat4*)(newData.scale + count);
    newData.modelMatrices = newData.localMatrices + count;
    newData.parent = (Id::ComponentIndex*)(newData.modelMatrices + count);
    newData.firstChild = newData.parent + count;
    newData.nextSibling = newData.firstChild + count;
    newData.prevSibling = newData.nextSibling + count;

    if (m_data.currentCount > 0) {
        memcpy(newData.entity, m_data.entity, m_data.currentCount * sizeof(Id::EntityId));
        memcpy(newData.pos, m_data.pos, m_data.currentCount * sizeof(glm::vec3));
        memcpy(newData.rot, m_data.rot, m_data.currentCount * sizeof(glm::vec3));
        memcpy(newData.scale, m_data.scale, m_data.currentCount * sizeof(glm::vec3));
        memcpy(newData.localMatrices, m_data.localMatrices, m_data.currentCount * sizeof(glm::mat4));
        memcpy(newData.modelMatrices, m_data.modelMatrices, m_data.currentCount * sizeof(glm::mat4));
        memcpy(newData.parent, m_data.parent, m_data.currentCount * sizeof(Id::ComponentIndex));
        memcpy(newData.firstChild, m_data.firstChild, m_data.currentCount * sizeof(Id::ComponentIndex));
        memcpy(newData.nextSibling, m_data.nextSibling, m_data.currentCount * sizeof(Id::ComponentIndex));
        memcpy(newData.prevSibling, m_data.prevSibling, m_data.currentCount * sizeof(Id::ComponentIndex));
    }

    free(m_data.buffer);

    m_data = newData;
}

void TransformManager::create(Id::EntityId entityId) {
    assert(!hasComponent(entityId));

    Id::idType index = Id::indexOf(entityId);
    if (index >= m_map.size())
        m_map.resize(index + 1, Id::invalidIndex);

    if (m_data.currentCount >= m_data.allocated)
        allocate(m_data.allocated == 0 ? 32 : m_data.allocated * 2);

    Id::ComponentIndex compIndex = m_data.currentCount;

    m_data.entity[compIndex] = entityId;
    m_data.pos[compIndex] = glm::vec3(0.0f);
    m_data.rot[compIndex] = glm::vec3(0.0f);
    m_data.scale[compIndex] = glm::vec3(1.0f);
    m_data.localMatrices[compIndex] = glm::mat4(1.0f);
    m_data.modelMatrices[compIndex] = glm::mat4(1.0f);

    m_data.parent[compIndex] = Id::invalidIndex;
    m_data.firstChild[compIndex] = Id::invalidIndex;
    m_data.nextSibling[compIndex] = Id::invalidIndex;
    m_data.prevSibling[compIndex] = Id::invalidIndex;

    m_map[index] = compIndex;
    ++m_data.currentCount;
}

void TransformManager::destroy(Id::EntityId entityId) {
    Id::ComponentIndex compIndex = getComponentIndex(entityId);

    // Get grandparent entity ID (could be Id::invalidId, representing the root scene)
    Id::EntityId grandparentEntityId = Id::invalidId;
    Id::ComponentIndex pIdx = m_data.parent[compIndex];
    if (pIdx != Id::invalidIndex) {
        grandparentEntityId = m_data.entity[pIdx];
    }

    // Promote children of this entity to be children of its parent
    Id::ComponentIndex childIdx = m_data.firstChild[compIndex];
    while (childIdx != Id::invalidIndex) {
        Id::ComponentIndex nextChildIdx = m_data.nextSibling[childIdx];
        Id::EntityId childEntityId = m_data.entity[childIdx];
        
        setParent(childEntityId, grandparentEntityId);
        
        childIdx = nextChildIdx;
    }
    m_data.firstChild[compIndex] = Id::invalidIndex;

    setParent(entityId, Id::invalidId);

    Id::ComponentIndex lastIndex = m_data.currentCount - 1;
    if (compIndex != lastIndex) {
        m_data.entity[compIndex] = m_data.entity[lastIndex];
        m_data.pos[compIndex] = m_data.pos[lastIndex];
        m_data.rot[compIndex] = m_data.rot[lastIndex];
        m_data.scale[compIndex] = m_data.scale[lastIndex];
        m_data.localMatrices[compIndex] = m_data.localMatrices[lastIndex];
        m_data.modelMatrices[compIndex] = m_data.modelMatrices[lastIndex];
        m_data.parent[compIndex] = m_data.parent[lastIndex];
        m_data.firstChild[compIndex] = m_data.firstChild[lastIndex];
        m_data.nextSibling[compIndex] = m_data.nextSibling[lastIndex];
        m_data.prevSibling[compIndex] = m_data.prevSibling[lastIndex];

        m_map[Id::indexOf(m_data.entity[compIndex])] = compIndex;

        // Fix references pointing to lastIndex -> change them to compIndex
        Id::ComponentIndex cIdx = m_data.firstChild[compIndex];
        while (cIdx != Id::invalidIndex) {
            m_data.parent[cIdx] = compIndex;
            cIdx = m_data.nextSibling[cIdx];
        }

        Id::ComponentIndex parentIdx = m_data.parent[compIndex];
        if (parentIdx != Id::invalidIndex && m_data.firstChild[parentIdx] == lastIndex) {
            m_data.firstChild[parentIdx] = compIndex;
        }

        Id::ComponentIndex prevIdx = m_data.prevSibling[compIndex];
        if (prevIdx != Id::invalidIndex) {
            m_data.nextSibling[prevIdx] = compIndex;
        }

        Id::ComponentIndex nextIdx = m_data.nextSibling[compIndex];
        if (nextIdx != Id::invalidIndex) {
            m_data.prevSibling[nextIdx] = compIndex;
        }
    }

    m_map[Id::indexOf(entityId)] = Id::invalidIndex;
    m_data.currentCount--;
}

void TransformManager::setParent(Id::EntityId childEntityId, Id::EntityId parentEntityId, bool keepWorld) {
    assert(childEntityId != parentEntityId); // Cannot parent to itself

    Id::ComponentIndex cIdx = getComponentIndex(childEntityId);

    glm::mat4 worldMat = m_data.modelMatrices[cIdx];

    // 1. Unlink from old parent/siblings
    Id::ComponentIndex oldPIdx = m_data.parent[cIdx];
    if (oldPIdx != Id::invalidIndex) {
        if (m_data.firstChild[oldPIdx] == cIdx) {
            m_data.firstChild[oldPIdx] = m_data.nextSibling[cIdx];
        }

        Id::ComponentIndex prev = m_data.prevSibling[cIdx];
        Id::ComponentIndex next = m_data.nextSibling[cIdx];

        if (prev != Id::invalidIndex) {
            m_data.nextSibling[prev] = next;
        }
        if (next != Id::invalidIndex) {
            m_data.prevSibling[next] = prev;
        }

        m_data.parent[cIdx] = Id::invalidIndex;
        m_data.prevSibling[cIdx] = Id::invalidIndex;
        m_data.nextSibling[cIdx] = Id::invalidIndex;
    }

    // 2. Link to new parent
    if (parentEntityId != Id::invalidId) {
        Id::ComponentIndex newPIdx = getComponentIndex(parentEntityId);
        
        m_data.parent[cIdx] = newPIdx;
        
        Id::ComponentIndex oldFirst = m_data.firstChild[newPIdx];
        m_data.nextSibling[cIdx] = oldFirst;
        m_data.prevSibling[cIdx] = Id::invalidIndex;
        
        if (oldFirst != Id::invalidIndex) {
            m_data.prevSibling[oldFirst] = cIdx;
        }
        
        m_data.firstChild[newPIdx] = cIdx;
    }

    // Adjust local transform vectors 
    if (keepWorld) {
        glm::mat4 newLocalMat;
        Id::ComponentIndex newPIdx = m_data.parent[cIdx];
        if (newPIdx == Id::invalidIndex)
            newLocalMat = worldMat;
        else
            newLocalMat = glm::inverse(m_data.modelMatrices[newPIdx]) * worldMat;

        // Decompose the new local matrix back into local scale, rotation, and translation
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::quat orientation;
        glm::decompose(newLocalMat, m_data.scale[cIdx], orientation, m_data.pos[cIdx], skew, perspective);
        
        // Convert orientation quaternion to Euler angles in degrees
        m_data.rot[cIdx] = glm::degrees(glm::eulerAngles(orientation));
        
        rebuildLocalMatrix(cIdx);
    }

    propagateTransform(cIdx);
}

void TransformManager::setPosition(Id::EntityId entityId, const glm::vec3& position) {
    Id::ComponentIndex index = getComponentIndex(entityId);
    m_data.pos[index] = position;
    rebuildLocalMatrix(index);
    propagateTransform(index);
}

glm::vec3 TransformManager::getPosition(Id::EntityId entityId) const {
    return m_data.pos[getComponentIndex(entityId)];
}

void TransformManager::setRotation(Id::EntityId entityId, const glm::vec3& rotation) {
    Id::ComponentIndex index = getComponentIndex(entityId);
    m_data.rot[index] = rotation;
    rebuildLocalMatrix(index);
    propagateTransform(index);
}

glm::vec3 TransformManager::getRotation(Id::EntityId entityId) const {
    return m_data.rot[getComponentIndex(entityId)];
}

void TransformManager::setScale(Id::EntityId entityId, const glm::vec3& scale) {
    Id::ComponentIndex index = getComponentIndex(entityId);
    m_data.scale[index] = scale;
    rebuildLocalMatrix(index);
    propagateTransform(index);
}

glm::vec3 TransformManager::getScale(Id::EntityId entityId) const {
    return m_data.scale[getComponentIndex(entityId)];
}

glm::mat4 TransformManager::getModelMatrix(Id::EntityId entityId) const {
    return m_data.modelMatrices[getComponentIndex(entityId)];
}

glm::mat4 TransformManager::getLocalMatrix(Id::EntityId entityId) const {
    return m_data.localMatrices[getComponentIndex(entityId)];
}

Id::EntityId TransformManager::getParent(Id::EntityId entityId) const {
    Id::ComponentIndex p = m_data.parent[getComponentIndex(entityId)];
    return (p == Id::invalidIndex) ? Id::invalidId : m_data.entity[p];
}

Id::EntityId TransformManager::getFirstChild(Id::EntityId entityId) const {
    Id::ComponentIndex fc = m_data.firstChild[getComponentIndex(entityId)];
    return (fc == Id::invalidIndex) ? Id::invalidId : m_data.entity[fc];
}

Id::EntityId TransformManager::getNextSibling(Id::EntityId entityId) const {
    Id::ComponentIndex ns = m_data.nextSibling[getComponentIndex(entityId)];
    return (ns == Id::invalidIndex) ? Id::invalidId : m_data.entity[ns];
}

void TransformManager::rebuildLocalMatrix(Id::ComponentIndex index) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_data.pos[index]);

    glm::vec3 r = m_data.rot[index];
    model = glm::rotate(model, glm::radians(r.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(r.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(r.z), glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, m_data.scale[index]);
    m_data.localMatrices[index] = model;
}

void TransformManager::propagateTransform(Id::ComponentIndex index) {
    Id::ComponentIndex pIdx = m_data.parent[index];
    if (pIdx == Id::invalidIndex)
        m_data.modelMatrices[index] = m_data.localMatrices[index];
    else
        m_data.modelMatrices[index] = m_data.modelMatrices[pIdx] * m_data.localMatrices[index];

    Id::ComponentIndex childIdx = m_data.firstChild[index];
    while (childIdx != Id::invalidIndex) {
        propagateTransform(childIdx);
        childIdx = m_data.nextSibling[childIdx];
    }
}

void TransformManager::update() {}

void TransformManager::onInspectorGui(Id::EntityId entityId) {
    if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen)) {
        glm::vec3 pos = getPosition(entityId);
        if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
            setPosition(entityId, pos);
        }

        glm::vec3 rot = getRotation(entityId);
        if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f)) {
            setRotation(entityId, rot);
        }

        glm::vec3 scale = getScale(entityId);
        if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
            setScale(entityId, scale);
        }
    }

    if (ImGui::CollapsingHeader("Scene Hierarchy Relations", ImGuiTreeNodeFlags_DefaultOpen)) {
        Id::EntityId parent = getParent(entityId);
        if (parent == Id::invalidId) {
            ImGui::Text("Parent: None (Root)");
        } else {
            std::string parentName = m_scene->getNameManager().getName(parent);
            ImGui::Text("Parent: %s [ID: %d]", parentName.c_str(), Id::indexOf(parent));
        }

        Id::EntityId firstChild = getFirstChild(entityId);
        if (firstChild == Id::invalidId) {
            ImGui::Text("First Child: None");
        } else {
            std::string childName = m_scene->getNameManager().getName(firstChild);
            ImGui::Text("First Child: %s [ID: %d]", childName.c_str(), Id::indexOf(firstChild));
        }

        Id::EntityId nextSibling = getNextSibling(entityId);
        if (nextSibling == Id::invalidId) {
            ImGui::Text("Next Sibling: None");
        } else {
            std::string siblingName = m_scene->getNameManager().getName(nextSibling);
            ImGui::Text("Next Sibling: %s [ID: %d]", siblingName.c_str(), Id::indexOf(nextSibling));
        }
    }
}
