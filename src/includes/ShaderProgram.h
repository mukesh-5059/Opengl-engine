#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class ShaderProgram {
    private:
        unsigned int m_id;
        void checkCompileErrors(unsigned int shader, std::string type);

    public:
        ShaderProgram(const char* vertexPath, const char* fragmentPath);
        ~ShaderProgram();

        void use();
        
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setVec4(const std::string& name, const glm::vec4& value) const;
        void setMat4(const std::string& name, const glm::mat4& mat) const;

        unsigned int getID() const { return m_id; }
};
