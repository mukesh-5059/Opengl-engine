#include <Material.h>
#include <Texture.h>
#include <ShaderProgram.h>
#include <ResourceManager.h>
#include <iostream>

Material::Material(std::shared_ptr<ShaderProgram> shader) 
    : m_shader(shader) {
    if (m_shader) {
        for (const auto& [name, loc] : m_shader->getFloatUniforms())
            m_floats[name] = Uniform<float>{ 0.0f, loc };

        for (const auto& [name, loc] : m_shader->getIntUniforms())
            m_ints[name] = Uniform<int>{ 0, loc };

        for (const auto& [name, loc] : m_shader->getVec3Uniforms())
            m_vec3s[name] = Uniform<glm::vec3>{ glm::vec3(0.0f), loc };

        for (const auto& [name, loc] : m_shader->getVec4Uniforms())
            m_vec4s[name] = Uniform<glm::vec4>{ glm::vec4(0.0f), loc };

        for (const auto& [name, loc] : m_shader->getMat4Uniforms())
            m_mat4s[name] = Uniform<glm::mat4>{ glm::mat4(1.0f), loc };

        for (const auto& [name, loc] : m_shader->getSamplerUniforms())
            m_textures[name] = TextureUnit{ m_shader == ResourceManager::getErrorShader() ? ResourceManager::getErrorTexture() : ResourceManager::getDefaultTexture(), 0, loc };

    }
    std::cout << "[Material] Created dynamic material" << std::endl;
}

Material::~Material() {
    std::cout << "[Material] Destroyed dynamic material" << std::endl;
}

void Material::apply() const {
    if (m_shader) {
        m_shader->use();
        
        for (const auto& [name, uniform] : m_floats)
            m_shader->setFloat(uniform.location, uniform.value);

        for (const auto& [name, uniform] : m_ints)
            m_shader->setInt(uniform.location, uniform.value);

        for (const auto& [name, uniform] : m_vec3s)
            m_shader->setVec3(uniform.location, uniform.value);

        for (const auto& [name, uniform] : m_vec4s)
            m_shader->setVec4(uniform.location, uniform.value);

        for (const auto& [name, uniform] : m_mat4s)
            m_shader->setMat4(uniform.location, uniform.value);

        // Bind all textures and set their sampler slots
        for (const auto& [name, unit] : m_textures) {
            if (unit.texture) {
                unit.texture->bind(unit.slot);
                m_shader->setInt(unit.location, unit.slot);
            }
        }
    }
}

void Material::setFloat(const std::string& name, float value) {
    auto it = m_floats.find(name);
    if (it != m_floats.end()) it->second.value = value;
    else std::cerr << "Warning: Uniform '" << name << "' is not supported by the shader." << std::endl;
}

void Material::setInt(const std::string& name, int value) {
    auto it = m_ints.find(name);
    if (it != m_ints.end()) it->second.value = value;
    else std::cerr << "Warning: Uniform '" << name << "' is not supported by the shader." << std::endl;
}

void Material::setVec3(const std::string& name, const glm::vec3& value) {
    auto it = m_vec3s.find(name);
    if (it != m_vec3s.end()) it->second.value = value;
    else std::cerr << "Warning: Uniform '" << name << "' is not supported by the shader." << std::endl;
}

void Material::setVec4(const std::string& name, const glm::vec4& value) {
    auto it = m_vec4s.find(name);
    if (it != m_vec4s.end()) it->second.value = value;
    else std::cerr << "Warning: Uniform '" << name << "' is not supported by the shader." << std::endl;
}

void Material::setMat4(const std::string& name, const glm::mat4& value) {
    auto it = m_mat4s.find(name);
    if (it != m_mat4s.end()) it->second.value = value;
    else std::cerr << "Warning: Uniform '" << name << "' is not supported by the shader." << std::endl;
}

void Material::setTexture(const std::string& name, std::shared_ptr<Texture> texture, int slot) {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        it->second.texture = texture;
        it->second.slot = slot;
    } else std::cerr << "Warning: Uniform sampler '" << name << "' is not supported by the shader." << std::endl;
}

bool Material::getFloat(const std::string& name, float& outValue) const {
    auto it = m_floats.find(name);
    if (it != m_floats.end()) {
        outValue = it->second.value;
        return true;
    }
    return false;
}

bool Material::getInt(const std::string& name, int& outValue) const {
    auto it = m_ints.find(name);
    if (it != m_ints.end()) {
        outValue = it->second.value;
        return true;
    }
    return false;
}

bool Material::getVec3(const std::string& name, glm::vec3& outValue) const {
    auto it = m_vec3s.find(name);
    if (it != m_vec3s.end()) {
        outValue = it->second.value;
        return true;
    }
    return false;
}

bool Material::getVec4(const std::string& name, glm::vec4& outValue) const {
    auto it = m_vec4s.find(name);
    if (it != m_vec4s.end()) {
        outValue = it->second.value;
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
