//
// Created by expleoene on 3/30/26.
//

#include "ResourceManager.h"

#include "../components/ModelComponent.h"

namespace holubiho {

    Texture *ResourceManager::GetTexture(const std::string &path) {
        auto it = Get().textures.find(path);
        if (it != Get().textures.end()) {
            return it->second.get();
        }
        return LoadTexture(path);
    }

    Texture *ResourceManager::LoadTexture(const std::string &path) {
        auto texture = std::make_unique<Texture>(path,1);
        auto* texPtr = texture.get();
        Get().textures.emplace(path, std::move(texture));
        return texPtr;
    }

    Texture *ResourceManager::LoadTexture(const std::string &path, GLint mode, GLint filterMode) {
        auto texture = std::make_unique<Texture>(path,1, mode, filterMode);
        auto* texPtr = texture.get();
        Get().textures.emplace(path, std::move(texture));
        return texPtr;
    }

    Texture *ResourceManager::CreateTexture(const std::string& name, int width, int height, bool mipmap, GLuint format) {
        auto texture = std::make_unique<Texture>( width, height, mipmap, format);
        auto* texPtr = texture.get();
        Get().textures.emplace(name, std::move(texture));
        return texPtr;
    }

    Texture* ResourceManager::LoadTextureCubeMap(const std::vector<std::string> &faces) {
        std::string key = "cube:";
        for (const std::string& face : faces) {
            key += face + ";";
        }

        auto it = Get().textures.find(key);
        if (it != Get().textures.end()) {
            return it->second.get();
        }
        auto texture = std::make_unique<Texture>(faces,1);
        Texture* texPtr = texture.get();
        Get().textures.emplace(key, std::move(texture));
        return texPtr;
    }


    Mesh* ResourceManager::LoadMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
        auto it = Get().meshes.find(name);
        if (it != Get().meshes.end()) {
            it->second->UpdateVertices(vertices);
            it->second->UpdateIndices(indices);
            return it->second.get();
        }

        auto mesh = std::make_unique<Mesh>(vertices, indices, name);
        Mesh* ptr = mesh.get();
        Get().meshes.emplace(name, std::move(mesh));

        return ptr;
    }


    Mesh* ResourceManager::GetMesh(const std::string &name){
        auto it = Get().meshes.find(name + ":0");
        if (it != Get().meshes.end()) {
            std::cout << "Mesh \"" << name << "\" found in cache." << std::endl;
            return it->second.get();
        }
        auto newMeshes = LoadMeshesFromFile(name);
        if (!newMeshes.empty()) return newMeshes[0];
        std::cout << "Mesh \"" << name << "\" not found." << std::endl;
        return nullptr;
    }

    Mesh *ResourceManager::GetMesh(const std::string &name, int idx) {
        auto it = Get().meshes.find(name + ":" + std::to_string(idx));
        if (it != Get().meshes.end()) {
            return it->second.get();
        }
        auto newMeshes = LoadMeshesFromFile(name);
        if (newMeshes.size() > idx) return newMeshes[idx];
        return nullptr;
    }


    std::vector<Mesh*> ResourceManager::LoadMeshesFromFile(const std::string &filepath) {

        const struct aiScene* ai_scene = aiImportFile( filepath.c_str(),
            aiProcess_CalcTangentSpace       |
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_SortByPType            |
            aiProcess_GenNormals             |
            aiProcess_OptimizeMeshes         | // Merges meshes sharing the same material
            aiProcess_OptimizeGraph);

        if( nullptr == ai_scene) {
            std::cerr << "Error loading the file: " << filepath << std::endl;
            return {};
        }
        auto newMeshes = Get().processAssimpSceneMeshes(ai_scene, filepath);

        aiReleaseImport(ai_scene);
        return newMeshes;
    }

    void ResourceManager::LoadModelsFromFile(const std::string &filepath, SceneObject* parent, bool asSeparateObjects) {

        const struct aiScene* ai_scene = aiImportFile( filepath.c_str(),
            aiProcess_CalcTangentSpace       |
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_SortByPType            |
            aiProcess_GenNormals             |
            aiProcess_OptimizeMeshes         | // Merges meshes sharing the same material
            aiProcess_OptimizeGraph);

        if( nullptr == ai_scene) {
            std::cerr << "Error loading the file: " << filepath << std::endl;
            return;
        }
         Get().processAssimpSceneObjects(ai_scene, filepath, parent, asSeparateObjects);

        aiReleaseImport(ai_scene);
    }

    ShaderProgram* ResourceManager::LoadShaderProgram(const std::string& name, const std::string& vertexFilepath, const std::string& fragmentFilepath) {
        auto it = Get().shaderPrograms.find(name);
        if (it != Get().shaderPrograms.end()) {
            return it->second.get();
        }

        auto shader = std::make_unique<ShaderProgram>(name, fragmentFilepath, vertexFilepath);
        ShaderProgram* ptr = shader.get();
        Get().shaderPrograms.emplace(name, std::move(shader));

        return ptr;
    }

    ShaderProgram *ResourceManager::GetShaderProgram(const std::string &name){
        auto it = Get().shaderPrograms.find(name);
        if (it != Get().shaderPrograms.end()) {
            return it->second.get();
        }
        return nullptr;
    }


    Material *ResourceManager::CreateMaterial(const std::string &name) {
        auto it = Get().materials.find(name);
        if (it != Get().materials.end()) {
            return it->second.get();
        }

        auto mat = std::make_unique<Material>();
        auto* ptr = mat.get();
        Get().materials.emplace(name, std::move(mat));
        return ptr;
    }

    Material *ResourceManager::GetMaterial(const std::string &name) {
        auto it = Get().materials.find(name);
        if (it != Get().materials.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    std::vector<Mesh *> ResourceManager::processAssimpSceneMeshes(const struct aiScene *ai_scene, const std::string &filepath){
        if (!ai_scene->HasMeshes()) return {};
        std::vector<Mesh*> newMeshes;

        for (size_t i = 0; i < ai_scene->mNumMeshes; i ++) {
            std::string obj_name = filepath + ":" + std::to_string(i);
            auto it = Get().meshes.find(obj_name);
            if (it != Get().meshes.end()) {
                newMeshes.push_back(it->second.get());
            }

            auto* ai_mesh = ai_scene->mMeshes[i];
            std::vector<Vertex> vertices;

            for (size_t j = 0; j < ai_mesh->mNumVertices; j++) {
                Vertex v{};
                v.position = glm::vec3(ai_mesh->mVertices[j].x, ai_mesh->mVertices[j].y, ai_mesh->mVertices[j].z);
                v.normal = glm::normalize(glm::vec3(ai_mesh->mNormals[j].x, ai_mesh->mNormals[j].y, ai_mesh->mNormals[j].z));
                if (ai_mesh->HasTextureCoords(0)) {
                    v.texCoords = glm::vec2(ai_mesh->mTextureCoords[0][j].x, ai_mesh->mTextureCoords[0][j].y);
                }
                if (ai_mesh->HasTangentsAndBitangents()) {
                    v.tangent = glm::vec3(ai_mesh->mTangents[j].x, ai_mesh->mTangents[j].y, ai_mesh->mTangents[j].z);
                    v.bitangent = glm::vec3(ai_mesh->mBitangents[j].x, ai_mesh->mBitangents[j].y, ai_mesh->mBitangents[j].z);
                }
                vertices.push_back(v);
            }

            std::vector<unsigned int> indices;

            for (int j = 0; j < ai_mesh->mNumFaces; j++) {
                auto ai_face = ai_mesh->mFaces[j];
                if (ai_face.mNumIndices != 3) continue;
                indices.push_back(ai_face.mIndices[0]);
                indices.push_back(ai_face.mIndices[1]);
                indices.push_back(ai_face.mIndices[2]);
            }

            auto mesh = std::make_unique<Mesh>(vertices,indices,obj_name);
            auto ptr = mesh.get();
            // mesh_comp->mesh = mesh.get();
            std::cout << "Created mesh " << obj_name << " with " << vertices.size() << " vertices" << std::endl;
            meshes.emplace(obj_name, std::move(mesh));
            newMeshes.push_back(ptr);
        }
        return newMeshes;
    }


    void ResourceManager::processAssimpSceneObjects(const struct aiScene *ai_scene, const std::string &filepath, SceneObject* parent, bool asSeparateObjects ) {
        if (!ai_scene->HasMeshes()) return;
        std::vector<Mesh*> newMeshes;
        std::vector<Material*> newMaterials;

        std::vector<unsigned int> meshMaterialIndices;

        if (ai_scene->HasMaterials()) {
            for (size_t i = 0; i < ai_scene->mNumMaterials; i++) {
                aiMaterial* ai_mat = ai_scene->mMaterials[i];
                std::string mat_name = filepath + "_mat_" + std::to_string(i);

                auto it = Get().materials.find(mat_name);
                if (it != Get().materials.end()) {
                    newMaterials.push_back(it->second.get());
                } else {
                    auto material = std::make_unique<Material>();

                    // Extract material properties here. Example: Diffuse Color
                    aiColor4D diffuse;
                    if (AI_SUCCESS == aiGetMaterialColor(ai_mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
                        material->diffuse = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
                        material->ambient = material->diffuse * 0.3f; // Simple ambient approximation
                    }

                    auto ptr = material.get();
                    materials.emplace(mat_name, std::move(material));
                    newMaterials.push_back(ptr);
                }
            }
        }

        for (size_t i = 0; i < ai_scene->mNumMeshes; i ++) {
            std::string obj_name = filepath + ":" + std::to_string(i);
            auto it = Get().meshes.find(obj_name);
            if (it != Get().meshes.end()) {
                newMeshes.push_back(it->second.get());
            }


            auto* ai_mesh = ai_scene->mMeshes[i];
            std::vector<Vertex> vertices;

            meshMaterialIndices.push_back(ai_mesh->mMaterialIndex);

            for (size_t j = 0; j < ai_mesh->mNumVertices; j++) {
                Vertex v{};
                v.position = glm::vec3(ai_mesh->mVertices[j].x, ai_mesh->mVertices[j].y, ai_mesh->mVertices[j].z);
                v.normal = glm::normalize(glm::vec3(ai_mesh->mNormals[j].x, ai_mesh->mNormals[j].y, ai_mesh->mNormals[j].z));
                if (ai_mesh->HasTextureCoords(0)) {
                    v.texCoords = glm::vec2(ai_mesh->mTextureCoords[0][j].x, ai_mesh->mTextureCoords[0][j].y);
                }
                if (ai_mesh->HasTangentsAndBitangents()) {
                    v.tangent = glm::vec3(ai_mesh->mTangents[j].x, ai_mesh->mTangents[j].y, ai_mesh->mTangents[j].z);
                    v.bitangent = glm::vec3(ai_mesh->mBitangents[j].x, ai_mesh->mBitangents[j].y, ai_mesh->mBitangents[j].z);
                }
                vertices.push_back(v);
            }

            std::vector<unsigned int> indices;

            for (int j = 0; j < ai_mesh->mNumFaces; j++) {
                auto ai_face = ai_mesh->mFaces[j];
                if (ai_face.mNumIndices != 3) continue;
                indices.push_back(ai_face.mIndices[0]);
                indices.push_back(ai_face.mIndices[1]);
                indices.push_back(ai_face.mIndices[2]);
            }

            auto mesh = std::make_unique<Mesh>(vertices,indices,obj_name);
            auto ptr = mesh.get();
            // mesh_comp->mesh = mesh.get();
            std::cout << "Created mesh " << obj_name << " with " << vertices.size() << " vertices" << std::endl;
            meshes.emplace(obj_name, std::move(mesh));
            newMeshes.push_back(ptr);
        }


        auto obj = parent;
        for (size_t i = 0; i < newMeshes.size(); i++) {
            Mesh* mesh = newMeshes[i];
            if (asSeparateObjects) {
                obj = parent->scene->CreateObject(mesh->name);
                parent->AddChild(obj);
            }

            auto model_comp = obj->AddComponent<ModelComponent>();
            model_comp->mesh = mesh;

            // Apply corresponding material
            if (!newMaterials.empty() && meshMaterialIndices[i] < newMaterials.size()) {
                model_comp->mat = newMaterials[meshMaterialIndices[i]];
            }
        }

    }

}
