#include <Scene.h>
#include <Primitive.h>
#include <iostream>
#include <imgui/imgui.h>

Scene::Scene() {
    std::cout << "[Scene] Initializing test scene..." << std::endl;
    m_testShader = new ShaderProgram("res/shaders/texture_test.vert", "res/shaders/texture_test.frag");
    m_testMaterial = new Material(m_testShader);
    m_testTexture = new Texture("res/textures/container_diffuse.png");
    m_testTexture1 = new Texture("res/textures/container_emmision.png");
    
    // Set default material properties
    m_testMaterial->setTexture("u_Texture", m_testTexture, 0);
    m_testMaterial->setTexture("u_Texture1", m_testTexture1, 1);
    m_testMaterial->setInt("u_UseTexture", 1);
    m_testMaterial->setVec3("u_BaseColor", glm::vec3(1));
    
    m_testMesh = Primitive::createCube(); 
    m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
}

Scene::~Scene() {
    std::cout << "[Scene] Cleaning up scene resources..." << std::endl;
    delete m_testTexture;
    delete m_testTexture1;
    delete m_testMaterial;
    delete m_testShader;
    delete m_testMesh;
    delete m_camera;
}

void Scene::update(float deltaTime, GLFWwindow* window) {
    if (m_camera) {
        m_camera->captureInput(window, deltaTime);
    }
}

void Scene::render(int width, int height) {
    if (m_testMaterial && m_testMesh && m_camera) {
        glm::mat4 model = glm::mat4(1.0f); // Identity matrix for now
        glm::mat4 view = m_camera->getViewMatrix();
        glm::mat4 projection = m_camera->getProjectionMatrix(width, height);

        m_testMaterial->setMat4("model", model);
        m_testMaterial->setMat4("view", view);
        m_testMaterial->setMat4("projection", projection);

        m_testMaterial->apply();
        m_testMesh->draw();
    }
}

void Scene::onGui() {
    if (m_camera) {
        m_camera->onGui();
    }

    if (m_testMaterial && ImGui::CollapsingHeader("Material Settings")) {
        glm::vec3 color;
        m_testMaterial->getVec3("u_BaseColor", color);
        if (ImGui::ColorEdit3("Base Color", (float*)&color)) {
            m_testMaterial->setVec3("u_BaseColor", color);
        }
        
        static bool useTexture = true;
        if (ImGui::Checkbox("Use Texture", &useTexture)) {
            m_testMaterial->setInt("u_UseTexture", useTexture ? 1 : 0);
        }
    }
}
