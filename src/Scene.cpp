#include <Scene.h>
#include <ResourceManager.h>
#include <Entity.h>
#include <Material.h>
#include <Renderer.h>

#include <iostream>
#include <imgui/imgui.h>
#include <memory>
#include <string>

Scene::Scene() : m_entityManager(this), m_transformManager(this) {
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
    
    m_camera = new Camera(glm::vec3(-29.837f, 15.676f, 18.146f));
}

Scene::~Scene() {
    std::cout << "[Scene] Cleaning up scene resources..." << std::endl;
    delete m_camera;
}

void Scene::update(float deltaTime, GLFWwindow* window) {
    if (m_camera) {
        m_camera->captureInput(window, deltaTime);
    }

    //float time = (float)glfwGetTime();
    //if (m_entities.size() >= 2) {
    //    m_entities[0]->setRotation(glm::vec3(0.0f, time * 20.0f, 0.0f));
    //    m_entities[1]->setRotation(glm::vec3(time * 30.0f, time * 30.0f, 0.0f));
    //}
}

void Scene::render(Renderer* renderer, int width, int height) {
    if (m_camera && renderer) {
        renderer->beginScene(m_camera, width, height);
        
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

    std::string label = "Entity " + std::to_string(Id::indexOf(entityId)) + " [Gen " + std::to_string(Id::generationOf(entityId)) + "]";
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
    if (ImGui::TreeNodeEx("New ECS Scene Graph", ImGuiTreeNodeFlags_DefaultOpen)) {
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
    if (m_camera) {
        m_camera->onGui();
        ImGui::Separator();
    }

    if (m_selectedEntityId != Id::invalidId) {
        ImGui::Text("Selected ECS Entity: ID %d (Gen %d)", Id::indexOf(m_selectedEntityId), Id::generationOf(m_selectedEntityId));
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen)) {
            glm::vec3 pos = m_transformManager.getPosition(m_selectedEntityId);
            if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
                m_transformManager.setPosition(m_selectedEntityId, pos);
            }

            glm::vec3 rot = m_transformManager.getRotation(m_selectedEntityId);
            if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f)) {
                m_transformManager.setRotation(m_selectedEntityId, rot);
            }

            glm::vec3 scale = m_transformManager.getScale(m_selectedEntityId);
            if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
                m_transformManager.setScale(m_selectedEntityId, scale);
            }
        }

        if (ImGui::CollapsingHeader("Scene Hierarchy Relations", ImGuiTreeNodeFlags_DefaultOpen)) {
            Id::EntityId parent = m_transformManager.getParent(m_selectedEntityId);
            if (parent == Id::invalidId)
                ImGui::Text("Parent: None (Root)");
            else
                ImGui::Text("Parent: Entity %d", Id::indexOf(parent));

            Id::EntityId firstChild = m_transformManager.getFirstChild(m_selectedEntityId);
            if (firstChild == Id::invalidId) {
                ImGui::Text("First Child: None");
            } else {
                ImGui::Text("First Child: Entity %d", Id::indexOf(firstChild));
            }

            Id::EntityId nextSibling = m_transformManager.getNextSibling(m_selectedEntityId);
            if (nextSibling == Id::invalidId) {
                ImGui::Text("Next Sibling: None");
            } else {
                ImGui::Text("Next Sibling: Entity %d", Id::indexOf(nextSibling));
            }
        }
    }
}
