#include <Texture.h>
#include <stb/StbImage.h>
#include <iostream>

Texture::Texture(const std::string& path) : m_id(0), m_path(path), m_width(0), m_height(0), m_nrChannels(0) {
    glGenTextures(1, &m_id);
    
    // Flip textures vertically on load so they match OpenGL's coordinate system
    stbi_set_flip_vertically_on_load(true);
    
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (m_nrChannels == 1)
            format = GL_RED;
        else if (m_nrChannels == 3)
            format = GL_RGB;
        else if (m_nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, m_id);
        
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set default wrapping/filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "[Texture] Loaded: " << path << " (" << m_width << "x" << m_height << ", " << m_nrChannels << " channels)" << std::endl;
    } else {
        std::cerr << "[Texture] Failed to load texture at path: " << path << std::endl;
    }
}

Texture::Texture(const void* buffer, int bufferSize, const std::string& nameKey) : m_id(0), m_path(nameKey), m_width(0), m_height(0), m_nrChannels(0) {
    glGenTextures(1, &m_id);
    
    stbi_set_flip_vertically_on_load(true);
    
    unsigned char* data = stbi_load_from_memory((const stbi_uc*)buffer, bufferSize, &m_width, &m_height, &m_nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (m_nrChannels == 1)
            format = GL_RED;
        else if (m_nrChannels == 3)
            format = GL_RGB;
        else if (m_nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, m_id);
        
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "[Texture] Loaded embedded texture: " << nameKey << " (" << m_width << "x" << m_height << ", " << m_nrChannels << " channels)" << std::endl;
    } else {
        std::cerr << "[Texture] Failed to load embedded texture: " << nameKey << std::endl;
    }
}

Texture::~Texture() {
    if (m_id != 0) {
        glDeleteTextures(1, &m_id);
        std::cout << "[Texture] Deleted texture: " << m_path << std::endl;
    }
}

void Texture::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}
