#include <Scene.h>
#include <ResourceManager.h>
#include <Model.h>
#include <Material.h>
#include <iostream>
#include <imgui/imgui.h>

Scene::Scene() {
    std::cout << "[Scene] Initializing test scene..." << std::endl;
    
    auto defaultShader = ResourceManager::loadShader("default", "res/shaders/default.vert", "res/shaders/default.frag");
    auto mat = std::make_shared<Material>(defaultShader);
    
    auto testMesh = ResourceManager::loadMesh("LanternSphere", "baseSphere");
    std::vector<ModelMesh> modelMeshes = { { testMesh, mat } };
    m_testModel = std::make_shared<Model>(modelMeshes);
    
    m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
}

Scene::~Scene() {
    std::cout << "[Scene] Cleaning up scene resources..." << std::endl;
    delete m_camera;
}

void Scene::update(float deltaTime, GLFWwindow* window) {
    if (m_camera) {
        m_camera->captureInput(window, deltaTime);
    }
}

void Scene::render(int width, int height) {
    if (m_testModel && m_camera) {
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = m_camera->getViewMatrix();
        glm::mat4 projection = m_camera->getProjectionMatrix(width, height);

        m_testModel->draw(model, view, projection);
    }
}

void Scene::onGui() {
    if (m_camera) {
        m_camera->onGui();
    }
}
