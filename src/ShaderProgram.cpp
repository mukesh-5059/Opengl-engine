#include <ShaderProgram.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath) {
    m_isValid = true;
    std::cout << "[ShaderProgram] Creating from: " << vertexPath << " and " << fragmentPath << std::endl;
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        
        vShaderFile.close();
        fShaderFile.close();
        
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Shader Program
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    glLinkProgram(m_id);
    checkCompileErrors(m_id, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    cacheActiveUniforms();
}

ShaderProgram::~ShaderProgram() {
    std::cout << "[ShaderProgram] Deleting program ID: " << m_id << std::endl;
    glDeleteProgram(m_id);
}

void ShaderProgram::use() {
    glUseProgram(m_id);
}

void ShaderProgram::cacheActiveUniforms() {
    int numActiveUniforms = 0;
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

    int maxNameLength = 0;
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

    if (numActiveUniforms > 0 && maxNameLength > 0) {
        std::vector<char> nameBuffer(maxNameLength);
        for (int i = 0; i < numActiveUniforms; ++i) {
            int length = 0;
            int size = 0;
            unsigned int type = 0;
            glGetActiveUniform(m_id, i, maxNameLength, &length, &size, &type, nameBuffer.data());
            std::string name(nameBuffer.data(), length);

            int location = glGetUniformLocation(m_id, name.c_str());
            if (location == -1) continue;

            switch (type) {
                case GL_FLOAT:          m_floatUniforms[name] = location; break;
                case GL_INT:    
                case GL_BOOL:           m_intUniforms[name] = location; break;
                case GL_FLOAT_VEC3:     m_vec3Uniforms[name] = location; break;
                case GL_FLOAT_VEC4:     m_vec4Uniforms[name] = location; break;
                case GL_FLOAT_MAT4:     m_mat4Uniforms[name] = location; break;
                case GL_SAMPLER_2D:     m_samplerUniforms[name] = location; break;
                default:                break; // Unsupported type
                    
            }
        }
    }
}



void ShaderProgram::setBool(int location, bool value) const {
    glUniform1i(location, (int)value);
}

void ShaderProgram::setInt(int location, int value) const {
    glUniform1i(location, value);
}

void ShaderProgram::setFloat(int location, float value) const {
    glUniform1f(location, value);
}

void ShaderProgram::setVec3(int location, const glm::vec3& value) const {
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void ShaderProgram::setVec4(int location, const glm::vec4& value) const {
    glUniform4fv(location, 1, glm::value_ptr(value));
}

void ShaderProgram::setMat4(int location, const glm::mat4& mat) const {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    int logLength = 0;
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            m_isValid = false;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0) {
                char* infoLog = (char*)malloc(logLength);
                glGetShaderInfoLog(shader, logLength, NULL, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                free(infoLog);
            }
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            m_isValid = false;
            glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0) {
                char* infoLog = (char*)malloc(logLength);
                glGetProgramInfoLog(shader, logLength, NULL, infoLog);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                free(infoLog);
            }
        }
    }
}
