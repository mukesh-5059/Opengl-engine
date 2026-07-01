#include <core/ResourceManager.h>
#include <renderer/Texture.h>
#include <renderer/ShaderProgram.h>
#include <renderer/Mesh.h>
#include <renderer/Material.h>
#include <renderer/Primitive.h>

#include <iostream>
#include <unordered_map>
#include <imgui/imgui.h>

namespace {
    template<typename T>
    struct CachedResource {
        std::weak_ptr<T> ptr;
        std::string source;
    };

    std::unordered_map<std::string, CachedResource<Texture>> s_textures;
    std::unordered_map<std::string, CachedResource<ShaderProgram>> s_shaders;
    std::unordered_map<std::string, CachedResource<Mesh>> s_meshes;

    std::shared_ptr<ShaderProgram> s_defaultShader;
    std::shared_ptr<ShaderProgram> s_errorShader;
    std::shared_ptr<Texture> s_defaultTexture;
    std::shared_ptr<Texture> s_errorTexture;
    std::shared_ptr<Material> s_defaultMaterial;
}

namespace ResourceManager {

    void init() {
        s_textures.reserve(32);
        s_shaders.reserve(16);
        s_meshes.reserve(32);
        std::cout << "[ResourceManager] Namespace initialized and maps preallocated." << std::endl;

        s_defaultShader = std::make_shared<ShaderProgram>("res/shaders/default.vert", "res/shaders/default.frag");

        s_errorShader = std::make_shared<ShaderProgram>("res/shaders/error.vert", "res/shaders/error.frag");

        unsigned char whitePixel[] = { 255, 255, 255, 255 };
        s_defaultTexture = std::make_shared<Texture>(whitePixel, 1, 1, 4);

        unsigned char checkerboardPixels[] = {
            255, 0, 255,   0, 0, 0,       255, 0, 255,   0, 0, 0,
            0, 0, 0,       255, 0, 255,   0, 0, 0,       255, 0, 255,
            255, 0, 255,   0, 0, 0,       255, 0, 255,   0, 0, 0,
            0, 0, 0,       255, 0, 255,   0, 0, 0,       255, 0, 255
        };
        s_errorTexture = std::make_shared<Texture>(checkerboardPixels, 4, 4, 3);

        s_defaultMaterial = std::make_shared<Material>(s_defaultShader);
    }

    void cleanUp() {
        std::cout << "[ResourceManager] Namespace cleaning up and clearing maps..." << std::endl;
        s_textures.clear();
        s_shaders.clear();
        s_meshes.clear();
        s_defaultShader.reset();
        s_errorShader.reset();
        s_defaultTexture.reset();
        s_errorTexture.reset();
        s_defaultMaterial.reset();
    }

    void prune() {
        for (auto it = s_textures.begin(); it != s_textures.end();) {
            if (it->second.ptr.expired()) {
                it = s_textures.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = s_shaders.begin(); it != s_shaders.end();) {
            if (it->second.ptr.expired()) {
                it = s_shaders.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = s_meshes.begin(); it != s_meshes.end();) {
            if (it->second.ptr.expired()) {
                it = s_meshes.erase(it);
            } else {
                ++it;
            }
        }
    }

    void onGui() {
        if (ImGui::BeginTable("ResourcesTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Name (Key)");
            ImGui::TableSetupColumn("Source (Path / Primitive)");
            ImGui::TableSetupColumn("Refs", ImGuiTableColumnFlags_WidthFixed, 40.0f);
            ImGui::TableHeadersRow();

            // Textures
            for (const auto& [name, cached] : s_textures) {
                if (auto shared = cached.ptr.lock()) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Texture");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%s", name.c_str());
                    ImGui::TableSetColumnIndex(2); ImGui::Text("%s", cached.source.c_str());
                    ImGui::TableSetColumnIndex(3); ImGui::Text("%ld", cached.ptr.use_count() - 1);
                }
            }

            // Shaders
            for (const auto& [name, cached] : s_shaders) {
                if (auto shared = cached.ptr.lock()) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Shader");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%s", name.c_str());
                    ImGui::TableSetColumnIndex(2); ImGui::Text("%s", cached.source.c_str());
                    ImGui::TableSetColumnIndex(3); ImGui::Text("%ld", cached.ptr.use_count() - 1);
                }
            }

            // Meshes
            for (const auto& [name, cached] : s_meshes) {
                if (auto shared = cached.ptr.lock()) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Mesh");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%s", name.c_str());
                    ImGui::TableSetColumnIndex(2); ImGui::Text("%s", cached.source.c_str());
                    ImGui::TableSetColumnIndex(3); ImGui::Text("%ld", cached.ptr.use_count() - 1);
                }
            }

            ImGui::EndTable();
        }

        if (ImGui::Button("Force Prune Cache")) {
            prune();
            std::cout << "[ResourceManager] Manual prune executed." << std::endl;
        }
    }

    std::shared_ptr<Texture> loadTexture(const std::string& name, const std::string& path) {
        auto it = s_textures.find(name);
        if (it != s_textures.end())
            if (auto shared = it->second.ptr.lock())
                return shared;

        std::cout << "[ResourceManager] Loading texture: '" << name << "' from: " << path << std::endl;
        auto texture = std::make_shared<Texture>(path);
        if (!texture->isValid()) {
            std::cerr << "[ResourceManager] Failed to load texture '" << name << "' from: " << path << ". Returning error texture." << std::endl;
            s_textures[name] = { s_errorTexture, "FALLBACK to error Texture" };
            return s_errorTexture;
        }
        s_textures[name] = { texture, path };
        return texture;
    }

    std::shared_ptr<ShaderProgram> loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
        auto it = s_shaders.find(name);
        if (it != s_shaders.end())
            if (auto shared = it->second.ptr.lock())
                return shared;

        std::cout << "[ResourceManager] Compiling shader: '" << name << "' (" << vertexPath << ", " << fragmentPath << ")" << std::endl;
        auto shader = std::make_shared<ShaderProgram>(vertexPath.c_str(), fragmentPath.c_str());
        if (!shader->isValid()) {
            std::cerr << "[ResourceManager] Failed to compile shader '" << name << "'. Returning error shader." << std::endl;
            s_shaders[name] = { s_errorShader, "FALLBACK to Error Shader" };
            return s_errorShader;
        }
        s_shaders[name] = { shader, vertexPath + " | " + fragmentPath };
        return shader;
    }

    std::shared_ptr<Mesh> loadMesh(const std::string& name, const std::string& pathOrPrimitive) {
        auto it = s_meshes.find(name);
        if (it != s_meshes.end())
            if (auto shared = it->second.ptr.lock())
                return shared;

        std::shared_ptr<Mesh> mesh;
        if (pathOrPrimitive == "baseCube")          mesh = std::shared_ptr<Mesh>(Primitive::createCube());
        else if (pathOrPrimitive == "baseTriangle") mesh = std::shared_ptr<Mesh>(Primitive::createTriangle());
        else if (pathOrPrimitive == "baseQuad")     mesh = std::shared_ptr<Mesh>(Primitive::createQuad());
        else if (pathOrPrimitive == "baseSphere")   mesh = std::shared_ptr<Mesh>(Primitive::createSphere());
        else {
            std::cerr << "[ResourceManager] Warning: Mesh loading from file not supported yet: " << pathOrPrimitive << std::endl;
            return nullptr;
        }

        std::cout << "[ResourceManager] Caching mesh: '" << name << "' (" << pathOrPrimitive << ")" << std::endl;
        s_meshes[name] = { mesh, pathOrPrimitive };
        return mesh;
    }

    std::shared_ptr<Material> getDefaultMaterial() {
        return s_defaultMaterial;
    }

    std::shared_ptr<Texture> getDefaultTexture() {
        return s_defaultTexture;
    }

    std::shared_ptr<Texture> getErrorTexture() {
        return s_errorTexture;
    }

    std::shared_ptr<ShaderProgram> getErrorShader() {
        return s_errorShader;
    }
}