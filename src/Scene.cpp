#include <Scene.h>
#include <ResourceManager.h>
#include <Entity.h>
#include <Material.h>
#include <Renderer.h>

#include <iostream>
#include <imgui/imgui.h>
#include <memory>

Scene::Scene() {
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

void Scene::onHierarchyGui() {
    ImGui::Text("Entities (%d)", (int)m_entities.size());
    ImGui::Separator();

    if (ImGui::TreeNodeEx("Root Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
        for (size_t i = 0; i < m_entities.size(); ++i) {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
            if (m_selectedEntityIndex == (int)i) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }
            std::string label = "Entity " + std::to_string(i);
            bool opened = ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", label.c_str());
            if (ImGui::IsItemClicked()) {
                m_selectedEntityIndex = (int)i;
            }
            if (opened) {
                ImGui::TreePop();
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

    if (m_selectedEntityIndex >= 0 && m_selectedEntityIndex < (int)m_entities.size()) {
        auto& entity = m_entities[m_selectedEntityIndex];
        ImGui::Text("Selected Entity: ID %d", m_selectedEntityIndex);
        ImGui::Separator();

        // Transform component
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            glm::vec3 pos = entity->getPosition();
            if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
                entity->setPosition(pos);
            }

            glm::vec3 rot = entity->getRotation();
            if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f)) {
                entity->setRotation(rot);
            }

            glm::vec3 scale = entity->getScale();
            if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
                entity->setScale(scale);
            }
        }

        // Mesh/Material Info
        if (ImGui::CollapsingHeader("Mesh & Material", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto mesh = entity->getMesh();
            if (mesh) {
                ImGui::Text("Mesh: Loaded");
            } else {
                ImGui::Text("Mesh: None");
            }

            auto material = entity->getMaterial();
            if (material) {
                ImGui::Text("Material: Loaded");
                auto shader = material->getShader();
                if (shader) {
                    ImGui::Text("Shader: Loaded");
                }
            } else {
                ImGui::Text("Material: None");
            }
        }
    } else {
        ImGui::Text("Select an entity to view details.");
    }
}
