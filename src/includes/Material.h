#pragma once

#include <ShaderProgram.h>
#include <glm/glm.hpp>

class Material {
    private:
        ShaderProgram* m_shader;
        glm::vec3 m_baseColor;

    public:
        Material(ShaderProgram* shader);
        ~Material();

        void apply() const;

        // Setters
        void setBaseColor(const glm::vec3& color) { m_baseColor = color; }
        
        // Getters
        ShaderProgram* getShader() const { return m_shader; }
        glm::vec3 getBaseColor() const { return m_baseColor; }
};
