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
    
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    glm::vec3 m_cameraPosition;

public:
    Renderer();
    ~Renderer() = default;

    void beginScene(Camera* camera, int width, int height);
    void submit(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4& transform);
    void submit(std::shared_ptr<Entity> entity);
    void endScene();
};
