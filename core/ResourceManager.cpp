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

//     Mesh* ResourceManager::LoadMesh(const std::string& name, const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
//     std::vector<Vertex> vertexData;
//     vertexData.reserve(vertices.size() / 3);
//
//     // 1. Create vertices and initialize normals to ZERO (important for accumulation)
//     for (size_t i = 0; i < vertices.size(); i += 3) {
//         Vertex v{};
//         v.position = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
//         v.normal = glm::vec3(0.0f); // Set to zero so we can add to it later
//         v.texCoords = glm::vec2(0.0f);
//         v.color = glm::vec3(1.0f);
//         vertexData.push_back(v);
//     }
//
//     // 2. Loop through every triangle to calculate and accumulate face normals
//     for (size_t i = 0; i < indices.size(); i += 3) {
//         unsigned int i0 = indices[i];
//         unsigned int i1 = indices[i + 1];
//         unsigned int i2 = indices[i + 2];
//
//         // Get the positions of the triangle's three vertices
//         glm::vec3 p0 = vertexData[i0].position;
//         glm::vec3 p1 = vertexData[i1].position;
//         glm::vec3 p2 = vertexData[i2].position;
//
//         // Calculate the two edges of the triangle
//         glm::vec3 edge1 = p1 - p0;
//         glm::vec3 edge2 = p2 - p0;
//
//         // Cross product gives us the perpendicular normal of the face.
//         // We do NOT normalize here. Leaving it un-normalized naturally weights
//         // the normal by the area of the triangle, yielding smoother results!
//         glm::vec3 faceNormal = glm::cross(edge1, edge2);
//
//         // Add this face normal to the vertex normals
//         vertexData[i0].normal += faceNormal;
//         vertexData[i1].normal += faceNormal;
//         vertexData[i2].normal += faceNormal;
//     }
//
//     // 3. Normalize the accumulated vectors to get the final smooth vertex normals
//     for (auto& v : vertexData) {
//         // Safe normalize: prevent division by zero for unused/degenerate vertices
//         if (glm::length(v.normal) > 0.00001f) {
//             v.normal = glm::normalize(v.normal);
//         } else {
//             v.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Fallback normal
//         }
//     }
//
//     return LoadMesh(name, vertexData, indices);
// }
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
