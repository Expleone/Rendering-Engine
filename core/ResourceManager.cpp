//
// Created by expleoene on 3/30/26.
//

#include "ResourceManager.h"

namespace BiBuild {
    Mesh* ResourceManager::LoadMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
        auto it = meshes.find(name);
        if (it != meshes.end()) {
            it->second->UpdateVertices(vertices);
            it->second->UpdateIndices(indices);
            return it->second.get();
        }

        auto mesh = std::make_unique<Mesh>(vertices, indices, name);
        Mesh* ptr = mesh.get();
        meshes.emplace(name, std::move(mesh));

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
    Mesh* ResourceManager::LoadMesh(const std::string& name, const float* vertices, const unsigned int vSize, const unsigned short* indices, const unsigned int iSize, const glm::vec3& color) {
        std::vector<Vertex> vertexData;
        for (size_t i = 0; i < vSize; i += 3) {
            Vertex v{};
            v.position = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
            v.color = color;
            vertexData.push_back(v);
        }
        std::vector<unsigned int> indexData(indices, indices + iSize);

        return LoadMesh(name, vertexData, indexData);
    }

    Mesh *ResourceManager::GetMesh(const std::string &name) const {
        auto it = meshes.find(name);
        if (it != meshes.end()) {
            return it->second.get();
        }
        return nullptr;
    }


    ShaderProgram* ResourceManager::LoadShaderProgram(const std::string& name, const std::string& vertexFilepath, const std::string& fragmentFilepath) {
        auto it = shaderPrograms.find(name);
        if (it != shaderPrograms.end()) {
            return it->second.get();
        }

        auto shader = std::make_unique<ShaderProgram>(fragmentFilepath, vertexFilepath);
        ShaderProgram* ptr = shader.get();
        shaderPrograms.emplace(name, std::move(shader));

        return ptr;
    }

    ShaderProgram *ResourceManager::GetShaderProgram(const std::string &name) const {
        auto it = shaderPrograms.find(name);
        if (it != shaderPrograms.end()) {
            return it->second.get();
        }
        return nullptr;
    }
}
