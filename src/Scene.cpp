#include <Scene.h>
#include <Primitive.h>
#include <iostream>

Scene::Scene() {
    std::cout << "[Scene] Initializing test scene..." << std::endl;
    m_testShader = new ShaderProgram("res/shaders/basic.vert", "res/shaders/basic.frag");
    m_testMesh = Primitive::createCube(); 
    m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
}

Scene::~Scene() {
    std::cout << "[Scene] Cleaning up scene resources..." << std::endl;
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
    if (m_testShader && m_testMesh && m_camera) {
        m_testShader->use();

        glm::mat4 model = glm::mat4(1.0f); // Identity matrix for now
        glm::mat4 view = m_camera->getViewMatrix();
        glm::mat4 projection = m_camera->getProjectionMatrix(width, height);

        m_testShader->setMat4("model", model);
        m_testShader->setMat4("view", view);
        m_testShader->setMat4("projection", projection);

        m_testMesh->draw();
    }
}

void Scene::onGui() {
    if (m_camera) {
        m_camera->onGui();
    }
}

