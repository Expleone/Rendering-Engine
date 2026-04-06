//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_RESOURCEMANAGER_H
#define VIEWER_RESOURCEMANAGER_H
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>


#include "ShaderProgram.h"
#include "Mesh.h"


namespace BiBuild {
    struct Vertex;

    class ResourceManager {
        std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;
        std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> shaderPrograms;


    public:
        ResourceManager() = default;

        Mesh* LoadMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        Mesh* LoadMesh(const std::string& name, const std::vector<float>& vertices, const std::vector<unsigned int>& indices); // Overload for simple models that use only positions
        Mesh* LoadMesh(const std::string& name, const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<unsigned int>& indices);
        Mesh* LoadMesh(const std::string& name, const float* vertices, const float* normals, const unsigned int vSize, const unsigned short* indices, const unsigned int iSize, const glm::vec3& color);
        Mesh* LoadMesh(const std::string &name, const float *vertices, unsigned int vSize, const unsigned short *indices, unsigned int iSize);
        Mesh* LoadMesh(const std::string &name, const float *vertices, unsigned int vSize, const unsigned short *indices, unsigned int iSize, const glm::vec3& color);
        Mesh* LoadMesh(const std::string& name, const std::string& filepath);
        Mesh* GetMesh(const std::string& name) const;

        ShaderProgram* LoadShaderProgram(const std::string& name, const std::string& vertexFilepath, const std::string& fragmentFilepath);
        ShaderProgram* GetShaderProgram(const std::string& name) const;

    };
}




#endif //VIEWER_RESOURSEMANAGER_H
