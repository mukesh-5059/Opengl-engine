#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
    private:
        unsigned int m_vao, m_vbo, m_ebo;
        unsigned int m_indexCount;

        void setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        ~Mesh();

        void draw() const;
};
