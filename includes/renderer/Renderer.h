#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class Mesh;
class Material;
class Entity;
class Camera;

struct RenderCommand {
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    glm::mat4 transform;
};

class Renderer {
private:
    std::vector<RenderCommand> m_drawQueue;
    unsigned int m_uboCamera;
    
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    glm::vec3 m_cameraPosition;

public:
    Renderer();
    ~Renderer();

    void beginScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& cameraPosition);
    void submit(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4& transform);
    void submit(std::shared_ptr<Entity> entity);
    void endScene();
};
