#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class ShaderProgram {
    private:
        unsigned int m_id;
        void checkCompileErrors(unsigned int shader, std::string type);

        std::unordered_map<std::string, int> m_floatUniforms;
        std::unordered_map<std::string, int> m_intUniforms;
        std::unordered_map<std::string, int> m_vec3Uniforms;
        std::unordered_map<std::string, int> m_vec4Uniforms;
        std::unordered_map<std::string, int> m_mat4Uniforms;
        std::unordered_map<std::string, int> m_samplerUniforms;

        void cacheActiveUniforms();

    public:
        ShaderProgram(const char* vertexPath, const char* fragmentPath);
        ~ShaderProgram();

        void use();
        
        void setBool(int location, bool value) const;
        void setInt(int location, int value) const;
        void setFloat(int location, float value) const;
        void setVec3(int location, const glm::vec3& value) const;
        void setVec4(int location, const glm::vec4& value) const;
        void setMat4(int location, const glm::mat4& mat) const;

        unsigned int getID() const { return m_id; }

        const std::unordered_map<std::string, int>& getFloatUniforms() const { return m_floatUniforms; }
        const std::unordered_map<std::string, int>& getIntUniforms() const { return m_intUniforms; }
        const std::unordered_map<std::string, int>& getVec3Uniforms() const { return m_vec3Uniforms; }
        const std::unordered_map<std::string, int>& getVec4Uniforms() const { return m_vec4Uniforms; }
        const std::unordered_map<std::string, int>& getMat4Uniforms() const { return m_mat4Uniforms; }
        const std::unordered_map<std::string, int>& getSamplerUniforms() const { return m_samplerUniforms; }
};

