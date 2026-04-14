//
// Created by expleoene on 3/30/26.
//

#include "ResourceManager.h"

#include "../components/ModelComponent.h"

namespace BiBuild {

    Texture *ResourceManager::GetTexture(const std::string &path) {
        auto it = Get().textures.find(path);
        if (it != Get().textures.end()) {
            return it->second.get();
        }
        return LoadTexture(path);
    }

    Texture *ResourceManager::LoadTexture(const std::string &path) {
        auto texture = std::make_unique<Texture>(path,0);
        auto* texPtr = texture.get();
        Get().textures.emplace(path, std::move(texture));
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

    Mesh* ResourceManager::LoadMesh(const std::string& name, const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
        std::vector<Vertex> vertexData;
        for (size_t i = 0; i < vertices.size(); i += 3) {
            Vertex v{};
            v.position = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
            vertexData.push_back(v);
        }
        return LoadMesh(name, vertexData, indices);
    }

    Mesh* ResourceManager::LoadMesh(const std::string& name, const float* vertices, const unsigned int vSize, const unsigned short* indices, const unsigned int iSize) {
        std::vector<Vertex> vertexData;
        for (size_t i = 0; i < vSize; i += 3) {
            Vertex v{};
            v.position = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
            vertexData.push_back(v);
        }
        std::vector<unsigned int> indexData(indices, indices + iSize);
        return LoadMesh(name, vertexData, indexData);
    }
    Mesh* ResourceManager::LoadMesh(const std::string& name, const float* vertices, const float* normals, const unsigned int vSize, const unsigned short* indices, const unsigned int iSize, const glm::vec3& color) {
        std::vector<Vertex> vertexData;
        vertexData.reserve(vSize); // Reserve memory for exactly 'vSize' vertices

        // Loop exactly 'vSize' times (e.g., 24 times)
        for (size_t i = 0; i < vSize; ++i) {
            Vertex v{};

            // The actual index in the flat float array is i * 3
            size_t idx = i * 3;

            v.position = glm::vec3(vertices[idx], vertices[idx + 1], vertices[idx + 2]);
            v.normal   = glm::vec3(normals[idx],  normals[idx + 1],  normals[idx + 2]);
            v.color    = color;

            // v.texCoords = glm::vec2(0.0f); // Good practice to initialize this too!

            vertexData.push_back(v);
        }

        std::vector<unsigned int> indexData(indices, indices + iSize);

        return LoadMesh(name, vertexData, indexData);
    }

    Mesh* ResourceManager::LoadMesh(const std::string& name, const float* vertices, const unsigned int vSize, const unsigned short* indices, const unsigned int iSize, const glm::vec3& color) {
        std::vector<Vertex> vertexData;
        vertexData.reserve(vSize / 3); // Good practice to reserve memory

        // 1. Initialize vertices with positions, color, and ZERO normals
        for (size_t i = 0; i < vSize; i += 3) {
            Vertex v{};
            v.position = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
            v.normal = glm::vec3(0.0f); // MUST be zero for accumulation to work
            v.color = color;
            // v.texCoords = glm::vec2(0.0f); // Assuming you initialize this elsewhere or leave default
            vertexData.push_back(v);
        }

        // 2. Loop through every triangle (groups of 3 indices) to calculate face normals
        for (size_t i = 0; i < iSize; i += 3) {
            unsigned short i0 = indices[i];
            unsigned short i1 = indices[i + 1];
            unsigned short i2 = indices[i + 2];

            // Get the positions of the triangle's three vertices
            glm::vec3 p0 = vertexData[i0].position;
            glm::vec3 p1 = vertexData[i1].position;
            glm::vec3 p2 = vertexData[i2].position;

            // Calculate the two edges of the triangle
            glm::vec3 edge1 = p1 - p0;
            glm::vec3 edge2 = p2 - p0;

            // Cross product gives us the perpendicular normal of the face.
            // Leaving it un-normalized naturally weights it by the area of the triangle!
            glm::vec3 faceNormal = glm::cross(edge1, edge2);

            // Add this face normal to the vertex normals
            vertexData[i0].normal += faceNormal;
            vertexData[i1].normal += faceNormal;
            vertexData[i2].normal += faceNormal;
        }

        // 3. Normalize the accumulated vectors to get the final smooth vertex normals
        for (auto& v : vertexData) {
            // Prevent division by zero for any unused/degenerate vertices
            if (glm::length(v.normal) > 0.00001f) {
                v.normal = glm::normalize(v.normal);
            } else {
                v.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Safe fallback
            }
        }

        // Convert the unsigned short array to std::vector<unsigned int> for your base function
        std::vector<unsigned int> indexData(indices, indices + iSize);

        return LoadMesh(name, vertexData, indexData);
    }

    Mesh* ResourceManager::GetMesh(const std::string &name){
        auto it = Get().meshes.find(name + ":0");
        if (it != Get().meshes.end()) {
            return it->second.get();
        }
        auto newMeshes = LoadMeshesFromFile(name);
        if (!newMeshes.empty()) return newMeshes[0];
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
            aiProcess_GenNormals);

        if( nullptr == ai_scene) {
            std::cerr << "Error loading the file: " << filepath << std::endl;
            return {};
        }
        auto newMeshes = Get().processAssimpScene(ai_scene, filepath);

        // We're done. Release all resources associated with this import
        aiReleaseImport(ai_scene);
        return newMeshes;
    }

    ShaderProgram* ResourceManager::LoadShaderProgram(const std::string& name, const std::string& vertexFilepath, const std::string& fragmentFilepath) {
        auto it = Get().shaderPrograms.find(name);
        if (it != Get().shaderPrograms.end()) {
            return it->second.get();
        }

        auto shader = std::make_unique<ShaderProgram>(fragmentFilepath, vertexFilepath);
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

    std::vector<Mesh *> ResourceManager::processAssimpScene(const struct aiScene *ai_scene, const std::string &filepath){
        if (!ai_scene->HasMeshes()) return {};
        std::vector<Mesh*> newMeshes;
        for (int i = 0; i < ai_scene->mNumMeshes; i ++) {
            std::string obj_name = filepath + ":" + std::to_string(i);

            auto* ai_mesh = ai_scene->mMeshes[i];
            std::vector<Vertex> vertices;

            for (int j = 0; j < ai_mesh->mNumVertices; j++) {
                Vertex v{};
                v.position = glm::vec3(ai_mesh->mVertices[j].x, ai_mesh->mVertices[j].y, ai_mesh->mVertices[j].z);
                v.normal = glm::normalize(glm::vec3(ai_mesh->mNormals[j].x, ai_mesh->mNormals[j].y, ai_mesh->mNormals[j].z));
                if (ai_mesh->HasTextureCoords(0)) {
                    v.texCoords = glm::vec2(ai_mesh->mTextureCoords[0][j].x, ai_mesh->mTextureCoords[0][j].y);
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

}
