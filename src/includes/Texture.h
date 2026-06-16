#pragma once

#include <glad/glad.h>
#include <string>

class Texture {
    private:
        unsigned int m_id;
        std::string m_path;
        int m_width, m_height, m_nrChannels;
    
    public:
        Texture(const std::string& path);
        Texture(const void* buffer, int bufferSize, const std::string& nameKey);
        ~Texture();
    
        void bind(unsigned int slot = 0) const;
        void unbind() const;
    
        unsigned int getID() const { return m_id; }
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }
        std::string getPath() const { return m_path; }
};
