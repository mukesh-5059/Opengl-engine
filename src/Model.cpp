#include <Model.h>
#include <Mesh.h>
#include <Material.h>
#include <iostream>

Model::Model(const std::vector<ModelMesh>& meshes) : m_meshes(meshes) {}

void Model::draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const {
    for (const auto& entry : m_meshes) {
        if (entry.mesh) {
            if (entry.material) {
                entry.material->setMat4("model", model);
                entry.material->apply();
            }
            entry.mesh->draw();
        }
    }
}

void Model::setMaterial(std::shared_ptr<Material> material, unsigned int index) {
    if (index < m_meshes.size()) {
        m_meshes[index].material = material;
    } else {
        std::cerr << "[Model] Error: Cannot set material, index " << index << " out of bounds (size: " << m_meshes.size() << ")" << std::endl;
    }
}

std::shared_ptr<Material> Model::getMaterial(unsigned int index) const {
    if (index < m_meshes.size()) {
        return m_meshes[index].material;
    }
    std::cerr << "[Model] Error: Cannot get material, index " << index << " out of bounds (size: " << m_meshes.size() << ")" << std::endl;
    return nullptr;
}
