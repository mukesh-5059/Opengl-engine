#pragma once

#include "ShaderProgram.h"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class ShaderProgram;
class Texture;

class Material {
    public:
        struct TextureUnit {
            Texture* texture;
            int slot;
        };

    private:
        ShaderProgram* m_shader;

        std::unordered_map<std::string, float> m_floats;
        std::unordered_map<std::string, int> m_ints;
        std::unordered_map<std::string, glm::vec3> m_vec3s;
        std::unordered_map<std::string, glm::vec4> m_vec4s;
        std::unordered_map<std::string, glm::mat4> m_mat4s;
        std::unordered_map<std::string, TextureUnit> m_textures;

    public:
        Material(ShaderProgram* shader);
        ~Material();

        void apply() const;

        void setFloat(const std::string& name, float value);
        void setInt(const std::string& name, int value);
        void setVec3(const std::string& name, const glm::vec3& value);
        void setVec4(const std::string& name, const glm::vec4& value);
        void setMat4(const std::string& name, const glm::mat4& value);
        void setTexture(const std::string& name, Texture* texture, int slot);

        bool getFloat(const std::string& name, float& outValue) const;
        bool getInt(const std::string& name, int& outValue) const;
        bool getVec3(const std::string& name, glm::vec3& outValue) const;
        bool getVec4(const std::string& name, glm::vec4& outValue) const;
        bool getTexture(const std::string& name, Texture*& outTexture, int& outSlot) const;
        
        ShaderProgram* getShader() const { return m_shader; }
};
