#include <Scene.h>
#include <Primitive.h>
#include <iostream>
#include <imgui/imgui.h>

Scene::Scene() {
    std::cout << "[Scene] Initializing test scene..." << std::endl;
    m_testShader = new ShaderProgram("res/shaders/basic.vert", "res/shaders/basic.frag");
    m_testMaterial = new Material(m_testShader);
    m_testMesh = Primitive::createSphere(); 
    m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
}

Scene::~Scene() {
    std::cout << "[Scene] Cleaning up scene resources..." << std::endl;
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
        m_testMaterial->apply();

        glm::mat4 model = glm::mat4(1.0f); // Identity matrix for now
        glm::mat4 view = m_camera->getViewMatrix();
        glm::mat4 projection = m_camera->getProjectionMatrix(width, height);

        m_testMaterial->getShader()->setMat4("model", model);
        m_testMaterial->getShader()->setMat4("view", view);
        m_testMaterial->getShader()->setMat4("projection", projection);

        m_testMesh->draw();
    }
}

void Scene::onGui() {
    if (m_camera) {
        m_camera->onGui();
    }

    if (m_testMaterial) {
        ImGui::Begin("Material Settings");
        glm::vec3 color = m_testMaterial->getBaseColor();
        if (ImGui::ColorEdit3("Base Color", (float*)&color)) {
            m_testMaterial->setBaseColor(color);
        }
        ImGui::End();
    }
}
