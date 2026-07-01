#include "Id.hpp"
#include <CameraManager.hpp>
#include <Scene.h>
#include <cassert>
#include <imgui/imgui.h>
#include <cstdlib>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>

CameraManager::CameraManager(Scene* scene) : m_scene(scene), m_map() {
    assert(m_scene != nullptr);
    m_data.currentCount = 0;
    m_data.allocated = 0;
    m_data.buffer = nullptr;
}

CameraManager::~CameraManager() {
    if (m_data.buffer) {
        free(m_data.buffer);
    }
}

void CameraManager::allocate(Id::ComponentIndex count) {
    assert(count >= m_data.currentCount);
    if (count <= m_data.allocated) return;

    // Round up allocation to a multiple of 16 to guarantee pointer alignment (16-byte alignment)
    count = (count + 15) & ~15;

    size_t bytes = count * (sizeof(Id::EntityId) + sizeof(float) * 4 + sizeof(bool) + sizeof(glm::mat4) * 2);

    DataInstance newData;
    newData.buffer = malloc(bytes);
    newData.currentCount = m_data.currentCount;
    newData.allocated = count;

    newData.entity = (Id::EntityId*)(newData.buffer);
    newData.fov = (float*)(newData.entity + count);
    newData.nearPlane = newData.fov + count;
    newData.farPlane = newData.nearPlane + count;
    newData.orthoSize = newData.farPlane + count;
    newData.isOrthographic = (bool*)(newData.orthoSize + count);
    newData.viewMatrix = (glm::mat4*)(newData.isOrthographic + count);
    newData.projectionMatrix = newData.viewMatrix + count;

    if (m_data.currentCount > 0) {
        memcpy(newData.entity, m_data.entity, m_data.currentCount * sizeof(Id::EntityId));
        memcpy(newData.fov, m_data.fov, m_data.currentCount * sizeof(float));
        memcpy(newData.nearPlane, m_data.nearPlane, m_data.currentCount * sizeof(float));
        memcpy(newData.farPlane, m_data.farPlane, m_data.currentCount * sizeof(float));
        memcpy(newData.orthoSize, m_data.orthoSize, m_data.currentCount * sizeof(float));
        memcpy(newData.isOrthographic, m_data.isOrthographic, m_data.currentCount * sizeof(bool));
        memcpy(newData.viewMatrix, m_data.viewMatrix, m_data.currentCount * sizeof(glm::mat4));
        memcpy(newData.projectionMatrix, m_data.projectionMatrix, m_data.currentCount * sizeof(glm::mat4));
    }

    free(m_data.buffer);
    m_data = newData;
}

void CameraManager::create(Id::EntityId entityId) {
    assert(!hasComponent(entityId));

    if (m_data.currentCount >= m_data.allocated)
        allocate(m_data.allocated == 0 ? 8 : m_data.allocated * 2);

    Id::ComponentIndex compIndex = m_data.currentCount;

    m_data.entity[compIndex] = entityId;
    m_data.fov[compIndex] = 45.0f;
    m_data.nearPlane[compIndex] = 0.1f;
    m_data.farPlane[compIndex] = 1000.0f;
    m_data.isOrthographic[compIndex] = false;
    m_data.orthoSize[compIndex] = 10.0f;
    m_data.viewMatrix[compIndex] = glm::mat4(1.0f);
    m_data.projectionMatrix[compIndex] = glm::mat4(1.0f);

    m_map[entityId] = compIndex;
    ++m_data.currentCount;
}

void CameraManager::destroy(Id::EntityId entityId) {

    Id::ComponentIndex compIndex = getComponentIndex(entityId);
    Id::ComponentIndex lastIndex = m_data.currentCount - 1;

    if (compIndex != lastIndex) {
        m_data.entity[compIndex] = m_data.entity[lastIndex];
        m_data.fov[compIndex] = m_data.fov[lastIndex];
        m_data.nearPlane[compIndex] = m_data.nearPlane[lastIndex];
        m_data.farPlane[compIndex] = m_data.farPlane[lastIndex];
        m_data.orthoSize[compIndex] = m_data.orthoSize[lastIndex];
        m_data.isOrthographic[compIndex] = m_data.isOrthographic[lastIndex];
        m_data.viewMatrix[compIndex] = m_data.viewMatrix[lastIndex];
        m_data.projectionMatrix[compIndex] = m_data.projectionMatrix[lastIndex];

        m_map[m_data.entity[compIndex]] = compIndex;
    }

    m_map.erase(entityId);
    --m_data.currentCount;
}

void CameraManager::setFOV(Id::EntityId entityId, float fov) {
    m_data.fov[getComponentIndex(entityId)] = fov;
}

float CameraManager::getFOV(Id::EntityId entityId) const {
    return m_data.fov[getComponentIndex(entityId)];
}

void CameraManager::setNearPlane(Id::EntityId entityId, float nearPlane) {
    m_data.nearPlane[getComponentIndex(entityId)] = nearPlane;
}

float CameraManager::getNearPlane(Id::EntityId entityId) const {
    return m_data.nearPlane[getComponentIndex(entityId)];
}

void CameraManager::setFarPlane(Id::EntityId entityId, float farPlane) {
    m_data.farPlane[getComponentIndex(entityId)] = farPlane;
}

float CameraManager::getFarPlane(Id::EntityId entityId) const {
    return m_data.farPlane[getComponentIndex(entityId)];
}

void CameraManager::setOrthographic(Id::EntityId entityId, bool isOrthographic) {
    m_data.isOrthographic[getComponentIndex(entityId)] = isOrthographic;
}

bool CameraManager::isOrthographic(Id::EntityId entityId) const {
    return m_data.isOrthographic[getComponentIndex(entityId)];
}

void CameraManager::setOrthoSize(Id::EntityId entityId, float size) {
    m_data.orthoSize[getComponentIndex(entityId)] = size;
}

float CameraManager::getOrthoSize(Id::EntityId entityId) const {
    return m_data.orthoSize[getComponentIndex(entityId)];
}

glm::mat4 CameraManager::getViewMatrix(Id::EntityId entityId) const {
    return m_data.viewMatrix[getComponentIndex(entityId)];
}

glm::mat4 CameraManager::getProjectionMatrix(Id::EntityId entityId) const {
    return m_data.projectionMatrix[getComponentIndex(entityId)];
}

void CameraManager::update(int viewportWidth, int viewportHeight) {
    float aspect = (float)viewportWidth / (float)viewportHeight;
    auto& transform = m_scene->getTransformManager();

    for (Id::ComponentIndex i = 0; i < m_data.currentCount; ++i) {
        Id::EntityId ent = m_data.entity[i];
        
        glm::vec3 pos = transform.getPosition(ent);
        glm::vec3 rot = transform.getRotation(ent);

        glm::mat4 camWorld = glm::translate(glm::mat4(1.0f), pos);
        camWorld = glm::rotate(camWorld, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
        camWorld = glm::rotate(camWorld, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
        camWorld = glm::rotate(camWorld, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

        m_data.viewMatrix[i] = glm::inverse(camWorld);

        if (m_data.isOrthographic[i]) {
            float size = m_data.orthoSize[i];
            float halfW = size * aspect * 0.5f;
            float halfH = size * 0.5f;
            m_data.projectionMatrix[i] = glm::ortho(-halfW, halfW, -halfH, halfH, m_data.nearPlane[i], m_data.farPlane[i]);
        } else {
            m_data.projectionMatrix[i] = glm::perspective(glm::radians(m_data.fov[i]), aspect, m_data.nearPlane[i], m_data.farPlane[i]);
        }
    }
}

void CameraManager::onInspectorGui(Id::EntityId entityId) {
    if (ImGui::CollapsingHeader("Camera Component", ImGuiTreeNodeFlags_DefaultOpen)) {
        bool isOrtho = isOrthographic(entityId);
        if (ImGui::Checkbox("Orthographic", &isOrtho))
            setOrthographic(entityId, isOrtho);

        if (isOrtho) {
            float size = getOrthoSize(entityId);
            if (ImGui::DragFloat("Ortho Size", &size, 0.1f, 0.1f, 100.0f))
                setOrthoSize(entityId, size);
        } else {
            float fov = getFOV(entityId);
            if (ImGui::DragFloat("FOV", &fov, 1.0f, 1.0f, 179.0f))
                setFOV(entityId, fov);
        }

        float nearPl = getNearPlane(entityId);
        if (ImGui::DragFloat("Near Plane", &nearPl, 0.05f, 0.01f, 10.0f))
            setNearPlane(entityId, nearPl);

        float farPl = getFarPlane(entityId);
        if (ImGui::DragFloat("Far Plane", &farPl, 1.0f, 1.0f, 10000.0f))
            setFarPlane(entityId, farPl);
    }
}
