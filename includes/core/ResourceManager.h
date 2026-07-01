#pragma once

#include <string>
#include <memory>

class Texture;
class ShaderProgram;
class Mesh;
class Material;

namespace ResourceManager {

    void init();
    void cleanUp();
    void prune();
    void onGui();

    std::shared_ptr<Texture> loadTexture(const std::string& name, const std::string& path);
    std::shared_ptr<ShaderProgram> loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    std::shared_ptr<Mesh> loadMesh(const std::string& name, const std::string& pathOrPrimitive);
    std::shared_ptr<Material> getDefaultMaterial();
    std::shared_ptr<Texture> getDefaultTexture();
    std::shared_ptr<Texture> getErrorTexture();
    std::shared_ptr<ShaderProgram> getErrorShader();
}
