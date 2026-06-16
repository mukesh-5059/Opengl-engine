#include <Scene.h>
#include <ResourceManager.h>
#include <Model.h>
#include <Material.h>
#include <iostream>
#include <imgui/imgui.h>

Scene::Scene() {
    std::cout << "[Scene] Initializing test scene..." << std::endl;
    
    // Create custom material for our test primitive
    auto defaultShader = ResourceManager::loadShader("default", "res/shaders/default.vert", "res/shaders/default.frag");
    auto mat = std::make_shared<Material>(defaultShader);
    
    auto tex = ResourceManager::loadTexture("Lantern_diffuse", "res/models/Lantern/T_LanternPole_a.png");
    auto tex1 = ResourceManager::loadTexture("Lantern_emission", "res/models/Lantern/T_LanternPole_e.png");
    
    mat->setTexture("u_DiffuseMap", tex, 0);
    mat->setTexture("u_EmissiveMap", tex1, 2);
    mat->setInt("u_UseDiffuseMap", 1);
    mat->setInt("u_UseEmissiveMap", 1);
    mat->setInt("u_UseSpecularMap", 0);
    mat->setVec3("u_BaseColor", glm::vec3(1.0f));
    
    // Instantiate our model via the primitive loader (not cached)
    m_testModel = ResourceManager::loadModel("Lantern", "res/models/Lantern/Lantern.fbx");
    m_testModel->setMaterial(mat);
    
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
        glm::mat4 model = glm::mat4(1.0f); // Identity matrix
        glm::mat4 view = m_camera->getViewMatrix();
        glm::mat4 projection = m_camera->getProjectionMatrix(width, height);

        // Draw model (handles binding shader, setting matrices, applying materials for all sub-meshes)
        m_testModel->draw(model, view, projection);
    }
}

void Scene::onGui() {
    if (m_camera) {
        m_camera->onGui();
    }

    if (m_testModel && ImGui::CollapsingHeader("Material Settings")) {
        auto mat = m_testModel->getMaterial(0);
        if (mat) {
            glm::vec3 color;
            mat->getVec3("u_BaseColor", color);
            if (ImGui::ColorEdit3("Base Color", (float*)&color)) {
                mat->setVec3("u_BaseColor", color);
            }
            
            int useTexture = 0;
            mat->getInt("u_UseDiffuseMap", useTexture);
            bool useTex = (useTexture == 1);
            if (ImGui::Checkbox("Use Texture", &useTex)) {
                mat->setInt("u_UseDiffuseMap", useTex ? 1 : 0);
            }
        }
    }
}
