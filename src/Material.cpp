#include <Material.h>
#include <Texture.h>
#include <ShaderProgram.h>
#include <iostream>

Material::Material(std::shared_ptr<ShaderProgram> shader) 
    : m_shader(shader) {
    std::cout << "[Material] Created dynamic material" << std::endl;
}

Material::~Material() {
    std::cout << "[Material] Destroyed dynamic material" << std::endl;
}

void Material::apply() const {
    if (m_shader) {
        m_shader->use();
        
        for (const auto& [name, value] : m_floats) {
            m_shader->setFloat(name, value);
        }

        for (const auto& [name, value] : m_ints) {
            m_shader->setInt(name, value);
        }

        for (const auto& [name, value] : m_vec3s) {
            m_shader->setVec3(name, value);
        }

        for (const auto& [name, value] : m_vec4s) {
            m_shader->setVec4(name, value);
        }

        for (const auto& [name, value] : m_mat4s) {
            m_shader->setMat4(name, value);
        }

        // Bind all textures and set their sampler slots
        for (const auto& [name, unit] : m_textures) {
            if (unit.texture) {
                unit.texture->bind(unit.slot);
                m_shader->setInt(name, unit.slot);
            }
        }
    }
}

void Material::setFloat(const std::string& name, float value) {
    m_floats[name] = value;
}

void Material::setInt(const std::string& name, int value) {
    m_ints[name] = value;
}

void Material::setVec3(const std::string& name, const glm::vec3& value) {
    m_vec3s[name] = value;
}

void Material::setVec4(const std::string& name, const glm::vec4& value) {
    m_vec4s[name] = value;
}

void Material::setMat4(const std::string& name, const glm::mat4& value) {
    m_mat4s[name] = value;
}

void Material::setTexture(const std::string& name, std::shared_ptr<Texture> texture, int slot) {
    m_textures[name] = TextureUnit{ texture, slot };
}

bool Material::getFloat(const std::string& name, float& outValue) const {
    auto it = m_floats.find(name);
    if (it != m_floats.end()) {
        outValue = it->second;
        return true;
    }
    return false;
}

bool Material::getInt(const std::string& name, int& outValue) const {
    auto it = m_ints.find(name);
    if (it != m_ints.end()) {
        outValue = it->second;
        return true;
    }
    return false;
}

bool Material::getVec3(const std::string& name, glm::vec3& outValue) const {
    auto it = m_vec3s.find(name);
    if (it != m_vec3s.end()) {
        outValue = it->second;
        return true;
    }
    return false;
}

bool Material::getVec4(const std::string& name, glm::vec4& outValue) const {
    auto it = m_vec4s.find(name);
    if (it != m_vec4s.end()) {
        outValue = it->second;
        return true;
    }
    return false;
}

bool Material::getTexture(const std::string& name, std::shared_ptr<Texture>& outTexture, int& outSlot) const {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        outTexture = it->second.texture;
        outSlot = it->second.slot;
        return true;
    }
    return false;
}
