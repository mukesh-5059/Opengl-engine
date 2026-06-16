#pragma once
#include <memory>
#include <glm/glm.hpp>

class Mesh;
class Material;

class Entity {
    private:
        std::shared_ptr<Mesh> m_mesh;
        std::shared_ptr<Material> m_material;
    
        glm::vec3 m_position{0.0f};
        glm::vec3 m_rotation{0.0f};
        glm::vec3 m_scale{1.0f};
    
    public:
        Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
        ~Entity() = default;
    
        std::shared_ptr<Mesh> getMesh() const { return m_mesh; }
        std::shared_ptr<Material> getMaterial() const { return m_material; }
    
        void setPosition(const glm::vec3& pos) { m_position = pos; }
        glm::vec3 getPosition() const { return m_position; }
    
        void setRotation(const glm::vec3& rot) { m_rotation = rot; }
        glm::vec3 getRotation() const { return m_rotation; }
    
        void setScale(const glm::vec3& scale) { m_scale = scale; }
        glm::vec3 getScale() const { return m_scale; }
    
        glm::mat4 getTransformMatrix() const;
};
