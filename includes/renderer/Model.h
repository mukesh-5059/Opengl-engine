#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

class Mesh;
class Material;

struct ModelMesh {
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
};

class Model {
private:
    std::vector<ModelMesh> m_meshes;

public:
    Model() = default;
    Model(const std::vector<ModelMesh>& meshes);
    ~Model() = default;

    void draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const;

    void setMaterial(std::shared_ptr<Material> material, unsigned int index = 0);
    std::shared_ptr<Material> getMaterial(unsigned int index = 0) const;
    
    const std::vector<ModelMesh>& getMeshes() const { return m_meshes; }
};
