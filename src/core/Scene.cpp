#include <core/Scene.h>
#include <core/ResourceManager.h>
#include <Entity.h>
#include <renderer/Material.h>
#include <renderer/Renderer.h>

#include <iostream>
#include <imgui/imgui.h>
#include <memory>
#include <string>

Scene::Scene() : m_entityManager(this), m_transformManager(this), m_nameManager(this), m_cameraManager(this) {
    std::cout << "[Scene] Initializing test scene..." << std::endl;
    
    auto baseShader = ResourceManager::loadShader("baseShader", "res/shaders/lit.vert", "res/shaders/lit.frag");
    auto cubeMesh = ResourceManager::loadMesh("cube", "baseCube");
    auto cubeMat = std::make_shared<Material>(baseShader);
    auto diffuse = ResourceManager::loadTexture("containerDiffuse", "res/textures/container_difuse.png");
    auto specular = ResourceManager::loadTexture("containerSpecular", "res/textures/container_specular.png");
    cubeMat->setTexture("u_DiffuseMap", diffuse, 0);
    cubeMat->setTexture("u_SpecularMap", specular, 1);
    
    int n = 10;
    for(int i = 0; i<n; ++i) for(int j = 0; j<n; ++j) for(int k = 0; k<n; k++){
        auto cubeEntity = std::make_shared<Entity>(cubeMesh, cubeMat);
        cubeEntity->setPosition(glm::vec3(2*i, 2*j, 2*k));
        m_entities.push_back(cubeEntity);
    }
    
    m_mainCameraEntity = m_entityManager.create("MainCamera");
    m_transformManager.setPosition(m_mainCameraEntity, glm::vec3(0.0f, 0.0f, 30.0f));
    m_cameraManager.create(m_mainCameraEntity);
}

Scene::~Scene() {
    std::cout << "[Scene] Cleaning up scene resources..." << std::endl;
}

void Scene::update(float deltaTime, GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    m_cameraManager.update(width, height);
}

void Scene::render(Renderer* renderer, int width, int height) {
    if (renderer && m_cameraManager.hasComponent(m_mainCameraEntity)) {
        glm::mat4 view = m_cameraManager.getViewMatrix(m_mainCameraEntity);
        glm::mat4 proj = m_cameraManager.getProjectionMatrix(m_mainCameraEntity);
        glm::vec3 pos = m_transformManager.getPosition(m_mainCameraEntity);
        renderer->beginScene(view, proj, pos);
        
        for (const auto& entity : m_entities) {
            if (entity) {
                renderer->submit(entity);
            }
        }
        
        renderer->endScene();
    }
}

void Scene::drawHierarchyNode(Id::EntityId entityId) {
    Id::EntityId firstChild = m_transformManager.getFirstChild(entityId);
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (firstChild == Id::invalidId)
        flags |= ImGuiTreeNodeFlags_Leaf;
    if (m_selectedEntityId == entityId)
        flags |= ImGuiTreeNodeFlags_Selected;

    std::string label = m_nameManager.getName(entityId) + " [ID: " + std::to_string(entityId) + "]";
    bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entityId, flags, "%s", label.c_str());
    
    if (ImGui::IsItemClicked()) m_selectedEntityId = entityId;

    if (opened) {
        Id::EntityId child = firstChild;
        while (child != Id::invalidId) {
            drawHierarchyNode(child);
            child = m_transformManager.getNextSibling(child);
        }
        ImGui::TreePop();
    }
}

void Scene::onHierarchyGui() {
    // 1. Random Spawn / Delete Button
    if (ImGui::Button("Spawn/Delete Random ECS Entities", ImVec2(-1, 0))) {
        // Spawn a random number of entities (1 to 4)
        int spawnCount = rand() % 4 + 1;
        for (int i = 0; i < spawnCount; ++i) {
            std::string name = "ProcAgent_" + std::to_string(rand() % 1000);
            Id::EntityId newEntity = m_entityManager.create(name); // MANDATORY name passed here!
            
            // Random position in [-10, 10]
            float x = (float)(rand() % 200 - 100) / 10.0f;
            float y = (float)(rand() % 200 - 100) / 10.0f;
            float z = (float)(rand() % 200 - 100) / 10.0f;
            m_transformManager.setPosition(newEntity, glm::vec3(x, y, z));
            
            // Randomly parent to an existing entity
            if (m_transformManager.getEntityCount() > 1 && (rand() % 100 < 60)) {
                // Find a random parent
                Id::ComponentIndex randomIdx = rand() % (m_transformManager.getEntityCount() - 1);
                Id::EntityId parentEntity = m_transformManager.getEntity(randomIdx);
                if (parentEntity != newEntity) {
                    m_transformManager.setParent(newEntity, parentEntity);
                }
            }
        }

        // Delete a random number of entities (1 to 2)
        if (m_transformManager.getEntityCount() > 0 && (rand() % 100 < 50)) {
            int deleteCount = rand() % 2 + 1;
            for (int i = 0; i < deleteCount; ++i) {
                if (m_transformManager.getEntityCount() == 0) break;
                Id::ComponentIndex randomIdx = rand() % m_transformManager.getEntityCount();
                Id::EntityId targetEntity = m_transformManager.getEntity(randomIdx);
                
                if (m_selectedEntityId == targetEntity) {
                    m_selectedEntityId = Id::invalidId;
                }
                
                m_entityManager.destroy(targetEntity);
            }
        }
    }

    ImGui::Separator();

    std::string title = "Scene Graph (" + std::to_string(m_entityManager.getCount()) + ")";
    if (ImGui::TreeNodeEx(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        Id::ComponentIndex count = m_transformManager.getEntityCount();
        if (count == 0)
            ImGui::TextDisabled("No ECS entities active.");
        else {
            for (Id::ComponentIndex i = 0; i < count; ++i) {
                Id::EntityId ent = m_transformManager.getEntity(i);
                if (m_transformManager.getParent(ent) == Id::invalidId) {
                    drawHierarchyNode(ent);
                }
            }
        }
        ImGui::TreePop();
    }
}

void Scene::onInspectorGui() {
    if (m_selectedEntityId != Id::invalidId) {
        ImGui::Text("Selected Entity: ID %d (Index %d Gen %d)", m_selectedEntityId, Id::indexOf(m_selectedEntityId), Id::generationOf(m_selectedEntityId));
        ImGui::Separator();

        m_nameManager.onInspectorGui(m_selectedEntityId);
        m_transformManager.onInspectorGui(m_selectedEntityId);

        if (m_cameraManager.hasComponent(m_selectedEntityId))
            m_cameraManager.onInspectorGui(m_selectedEntityId);
    } else
        ImGui::Text("Select an entity to view details.");
}
