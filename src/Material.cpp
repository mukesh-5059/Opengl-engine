#include <Material.h>
#include <iostream>

Material::Material(ShaderProgram* shader) 
    : m_shader(shader), m_baseColor(glm::vec3(1.0f, 0.5f, 0.2f)) {
    std::cout << "[Material] Created" << std::endl;
}

Material::~Material() {
    std::cout << "[Material] Destroyed" << std::endl;
}

void Material::apply() const {
    if (m_shader) {
        m_shader->use();
        m_shader->setVec3("u_BaseColor", m_baseColor);
    }
}
