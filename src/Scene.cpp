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
    
    auto defaultShader = ResourceManager::loadShader("lit", "res/shaders/lit.vert", "res/shaders/lit.frag");
    auto cubeMat = std::make_shared<Material>(defaultShader);
    auto diffuse = ResourceManager::loadTexture("ContainerDiffuse", "res/textures/container_diffuse.png");
    auto specular = ResourceManager::loadTexture("ContainerSpecular", "res/textures/container_specular.png");
    auto cubeMesh = ResourceManager::loadMesh("CubeMesh", "baseCube");

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

void Scene::onGui() {
    if (m_camera) {
        m_camera->onGui();
    }

    for (size_t i = 0; i < m_entities.size(); ++i) {
        std::string label = "Entity " + std::to_string(i);
        if (ImGui::CollapsingHeader(label.c_str())) {
            glm::vec3 pos = m_entities[i]->getPosition();
            if (ImGui::DragFloat3("Position", (float*)&pos, 0.05f))
                m_entities[i]->setPosition(pos);

            glm::vec3 scale = m_entities[i]->getScale();
            if (ImGui::DragFloat3("Scale", (float*)&scale, 0.05f))
                m_entities[i]->setScale(scale);
        }
    }
}
