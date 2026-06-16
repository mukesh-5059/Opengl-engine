#include <ResourceManager.h>
#include <Texture.h>
#include <ShaderProgram.h>
#include <Mesh.h>
#include <Model.h>
#include <Material.h>
#include <Primitive.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <unordered_map>
#include <imgui/imgui.h>

#define ASSIMP_FLAGS (aiProcess_Triangulate  | aiProcess_PreTransformVertices | aiProcess_GenNormals)

namespace {
    template<typename T>
    struct CachedResource {
        std::weak_ptr<T> ptr;
        std::string source;
    };

    std::unordered_map<std::string, CachedResource<Texture>> s_textures;
    std::unordered_map<std::string, CachedResource<ShaderProgram>> s_shaders;
    std::unordered_map<std::string, CachedResource<Mesh>> s_meshes;
    std::unordered_map<std::string, CachedResource<Model>> s_models;
}

namespace ResourceManager {

    void init() {
        s_textures.reserve(32);
        s_shaders.reserve(16);
        s_meshes.reserve(32);
        s_models.reserve(32);
        std::cout << "[ResourceManager] Namespace initialized and maps preallocated." << std::endl;
    }

    void cleanUp() {
        std::cout << "[ResourceManager] Namespace cleaning up and clearing maps..." << std::endl;
        s_textures.clear();
        s_shaders.clear();
        s_meshes.clear();
        s_models.clear();
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

        for (auto it = s_models.begin(); it != s_models.end();) {
            if (it->second.ptr.expired()) {
                it = s_models.erase(it);
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

            // Models
            for (const auto& [name, cached] : s_models) {
                if (auto shared = cached.ptr.lock()) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Model");
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

    std::shared_ptr<Model> loadModel(const std::string& name, const std::string& path) {
        auto it = s_models.find(name);
        if (it != s_models.end())
            if (auto shared = it->second.ptr.lock())
                return shared;

        std::cout << "[ResourceManager] Loading model: '" << name << "' from: " << path << std::endl;

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, ASSIMP_FLAGS);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "[ResourceManager] Assimp Error loading model: " << importer.GetErrorString() << std::endl;
            return nullptr;
        }

        std::string directory = "";
        size_t lastSlash = path.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            directory = path.substr(0, lastSlash);
        }

        // Load Default Shader
        auto defaultShader = loadShader("default", "res/shaders/default.vert", "res/shaders/default.frag");

        // Parse Materials
        std::vector<std::shared_ptr<Material>> materials;
        materials.reserve(scene->mNumMaterials);

        for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
            aiMaterial* aiMat = scene->mMaterials[i];
            auto mat = std::make_shared<Material>(defaultShader);

            mat->setInt("u_UseDiffuseMap", 0);
            mat->setInt("u_UseSpecularMap", 0);
            mat->setInt("u_UseEmissiveMap", 0);
            mat->setVec3("u_BaseColor", glm::vec3(1.0f));

            // Get diffuse color
            aiColor3D color(1.f, 1.f, 1.f);
            if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
                mat->setVec3("u_BaseColor", glm::vec3(color.r, color.g, color.b));
            }

            // Check for diffuse texture
            if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString str;
                aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
                std::string texPath = str.C_Str();

                std::shared_ptr<Texture> texture;
                std::string texName = name + "_" + texPath;

                // Handle embedded texture
                if (texPath.size() > 0 && texPath[0] == '*') {
                    int embedIndex = std::stoi(texPath.substr(1));
                    if (embedIndex < (int)scene->mNumTextures) {
                        aiTexture* aiTex = scene->mTextures[embedIndex];
                        std::string embedName = name + "_embed_" + std::to_string(embedIndex);
                        
                        auto texIt = s_textures.find(embedName);
                        if (texIt != s_textures.end()) {
                            texture = texIt->second.ptr.lock();
                        }
                        
                        if (!texture) {
                            if (aiTex->mHeight == 0) {
                                texture = std::make_shared<Texture>(aiTex->pcData, aiTex->mWidth, embedName);
                                s_textures[embedName] = { texture, "Embedded: " + std::to_string(embedIndex) };
                            } else {
                                std::cerr << "[ResourceManager] Warning: Uncompressed raw embedded textures not supported yet." << std::endl;
                            }
                        }
                    }
                } else {
                    // Load from file relative to model path
                    std::string fullTexPath = directory.empty() ? texPath : directory + "/" + texPath;
                    texture = loadTexture(texName, fullTexPath);
                }

                if (texture) {
                    mat->setTexture("u_DiffuseMap", texture, 0);
                    mat->setInt("u_UseDiffuseMap", 1);
                }
            }

            // Check for specular texture
            if (aiMat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
                aiString str;
                aiMat->GetTexture(aiTextureType_SPECULAR, 0, &str);
                std::string texPath = str.C_Str();

                std::shared_ptr<Texture> texture;
                std::string texName = name + "_specular_" + texPath;

                if (texPath.size() > 0 && texPath[0] == '*') {
                    int embedIndex = std::stoi(texPath.substr(1));
                    if (embedIndex < (int)scene->mNumTextures) {
                        aiTexture* aiTex = scene->mTextures[embedIndex];
                        std::string embedName = name + "_embed_" + std::to_string(embedIndex);
                        
                        auto texIt = s_textures.find(embedName);
                        if (texIt != s_textures.end()) {
                            texture = texIt->second.ptr.lock();
                        }
                        
                        if (!texture && aiTex->mHeight == 0) {
                            texture = std::make_shared<Texture>(aiTex->pcData, aiTex->mWidth, embedName);
                            s_textures[embedName] = { texture, "Embedded Specular: " + std::to_string(embedIndex) };
                        }
                    }
                } else {
                    std::string fullTexPath = directory.empty() ? texPath : directory + "/" + texPath;
                    texture = loadTexture(texName, fullTexPath);
                }

                if (texture) {
                    mat->setTexture("u_SpecularMap", texture, 1);
                    mat->setInt("u_UseSpecularMap", 1);
                }
            }

            // Check for emissive texture
            if (aiMat->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
                aiString str;
                aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &str);
                std::string texPath = str.C_Str();

                std::shared_ptr<Texture> texture;
                std::string texName = name + "_emissive_" + texPath;

                if (texPath.size() > 0 && texPath[0] == '*') {
                    int embedIndex = std::stoi(texPath.substr(1));
                    if (embedIndex < (int)scene->mNumTextures) {
                        aiTexture* aiTex = scene->mTextures[embedIndex];
                        std::string embedName = name + "_embed_" + std::to_string(embedIndex);
                        
                        auto texIt = s_textures.find(embedName);
                        if (texIt != s_textures.end()) {
                            texture = texIt->second.ptr.lock();
                        }
                        
                        if (!texture && aiTex->mHeight == 0) {
                            texture = std::make_shared<Texture>(aiTex->pcData, aiTex->mWidth, embedName);
                            s_textures[embedName] = { texture, "Embedded Emissive: " + std::to_string(embedIndex) };
                        }
                    }
                } else {
                    std::string fullTexPath = directory.empty() ? texPath : directory + "/" + texPath;
                    texture = loadTexture(texName, fullTexPath);
                }

                if (texture) {
                    mat->setTexture("u_EmissiveMap", texture, 2);
                    mat->setInt("u_UseEmissiveMap", 1);
                }
            }

            materials.push_back(mat);
        }

        // Parse Meshes
        std::vector<ModelMesh> modelMeshes;
        modelMeshes.reserve(scene->mNumMeshes);

        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* aiMesh = scene->mMeshes[i];
            
            std::vector<Vertex> vertices;
            vertices.reserve(aiMesh->mNumVertices);
            std::vector<unsigned int> indices;

            for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                Vertex vertex;
                vertex.position = glm::vec3(aiMesh->mVertices[j].x, aiMesh->mVertices[j].y, aiMesh->mVertices[j].z);
                
                if (aiMesh->HasNormals()) {
                    vertex.normal = glm::vec3(aiMesh->mNormals[j].x, aiMesh->mNormals[j].y, aiMesh->mNormals[j].z);
                } else {
                    vertex.normal = glm::vec3(0.0f);
                }
                
                if (aiMesh->mTextureCoords[0]) {
                    vertex.texCoords = glm::vec2(aiMesh->mTextureCoords[0][j].x, aiMesh->mTextureCoords[0][j].y);
                } else {
                    vertex.texCoords = glm::vec2(0.0f);
                }
                
                vertices.push_back(vertex);
            }

            for (unsigned int j = 0; j < aiMesh->mNumFaces; j++) {
                aiFace face = aiMesh->mFaces[j];
                for (unsigned int k = 0; k < face.mNumIndices; k++) {
                    indices.push_back(face.mIndices[k]);
                }
            }

            std::string meshName = name + "_mesh_" + std::to_string(i);
            
            // Cache Mesh
            auto mesh = std::make_shared<Mesh>(vertices, indices);
            s_meshes[meshName] = { mesh, path + " (mesh " + std::to_string(i) + ")" };

            std::shared_ptr<Material> meshMaterial = nullptr;
            if (aiMesh->mMaterialIndex < materials.size()) {
                meshMaterial = materials[aiMesh->mMaterialIndex];
            }

            modelMeshes.push_back({ mesh, meshMaterial });
        }

        auto model = std::make_shared<Model>(modelMeshes);
        s_models[name] = { model, path };
        return model;
    }

    std::shared_ptr<Model> createPrimitiveModel(const std::string& name, const std::string& primitiveType, std::shared_ptr<Material> material) {
        auto mesh = loadMesh(name + "_mesh", primitiveType);
        if (!mesh) {
            return nullptr;
        }

        std::shared_ptr<Material> finalMaterial = material;
        if (!finalMaterial) {
            auto defaultShader = loadShader("default", "res/shaders/default.vert", "res/shaders/default.frag");
            finalMaterial = std::make_shared<Material>(defaultShader);
            finalMaterial->setInt("u_UseDiffuseMap", 0);
            finalMaterial->setInt("u_UseSpecularMap", 0);
            finalMaterial->setInt("u_UseEmissiveMap", 0);
            finalMaterial->setVec3("u_BaseColor", glm::vec3(1.0f));
        }

        std::vector<ModelMesh> meshes = { { mesh, finalMaterial } };
        return std::make_shared<Model>(meshes);
    }
}
