#include <Renderer.h>
#include <Mesh.h>
#include <Material.h>
#include <Entity.h>
#include <Camera.h>

Renderer::Renderer() {
    m_drawQueue.reserve(1024);
}

void Renderer::beginScene(Camera* camera, int width, int height) {
    m_drawQueue.clear();
    if (camera) {
        m_viewMatrix = camera->getViewMatrix();
        m_projectionMatrix = camera->getProjectionMatrix(width, height);
        m_cameraPosition = camera->getPosition();
    }
}

void Renderer::submit(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4& transform) {
    m_drawQueue.push_back({ mesh, material, transform });
}

void Renderer::submit(std::shared_ptr<Entity> entity) {
    if (entity) {
        submit(entity->getMesh(), entity->getMaterial(), entity->getTransformMatrix());
    }
}

void Renderer::endScene() {
    for (const auto& command : m_drawQueue) {
        if (command.mesh && command.material) {
            command.material->setMat4("model", command.transform);
            command.material->setMat4("view", m_viewMatrix);
            command.material->setMat4("projection", m_projectionMatrix);
            command.material->setVec3("u_ViewPos", m_cameraPosition);
            
            command.material->apply();
            command.mesh->draw();
        }
    }
}
