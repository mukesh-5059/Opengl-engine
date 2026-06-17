#include <Renderer.h>
#include <Mesh.h>
#include <Material.h>
#include <Entity.h>
#include <Camera.h>
#include <glad/glad.h>

struct CameraData {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 u_ViewPos;
    float padding;
};

Renderer::Renderer() : m_uboCamera(0) {
    m_drawQueue.reserve(1024);
    glGenBuffers(1, &m_uboCamera);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uboCamera);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uboCamera);
}

Renderer::~Renderer() {
    if (m_uboCamera != 0)
        glDeleteBuffers(1, &m_uboCamera);
}

void Renderer::beginScene(Camera* camera, int width, int height) {
    m_drawQueue.clear();
    if (camera) {
        m_viewMatrix = camera->getViewMatrix();
        m_projectionMatrix = camera->getProjectionMatrix(width, height);
        m_cameraPosition = camera->getPosition();

        CameraData data;
        data.projection = m_projectionMatrix;
        data.view = m_viewMatrix;
        data.u_ViewPos = m_cameraPosition;
        data.padding = 0.0f;

        glBindBuffer(GL_UNIFORM_BUFFER, m_uboCamera);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraData), &data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

void Renderer::submit(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4& transform) {
    m_drawQueue.push_back({ mesh, material, transform });
}

void Renderer::submit(std::shared_ptr<Entity> entity) {
    if (entity)
        submit(entity->getMesh(), entity->getMaterial(), entity->getTransformMatrix());
}

void Renderer::endScene() {
    for (const auto& command : m_drawQueue) {
        if (command.mesh && command.material) {
            command.material->setMat4("model", command.transform);
            command.material->apply();
            command.mesh->draw();
        }
    }
}
