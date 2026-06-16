#include <Entity.h>
#include <Mesh.h>
#include <Material.h>
#include <glm/gtc/matrix_transform.hpp>

Entity::Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
    : m_mesh(mesh), m_material(material) {}

glm::mat4 Entity::getTransformMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_position);
    
    model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    
    model = glm::scale(model, m_scale);
    return model;
}

void Entity::draw(const glm::mat4& view, const glm::mat4& projection) const {
    if (m_mesh && m_material) {
        glm::mat4 modelMatrix = getTransformMatrix();
        glm::vec3 viewPos = glm::vec3(glm::inverse(view)[3]);

        m_material->setMat4("model", modelMatrix);
        m_material->setMat4("view", view);
        m_material->setMat4("projection", projection);
        m_material->setVec3("u_ViewPos", viewPos);
        m_material->apply();

        m_mesh->draw();
    }
}
