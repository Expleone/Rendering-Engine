//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_RESOURCEMANAGER_H
#define VIEWER_RESOURCEMANAGER_H
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/scene.h>          // Output data structure
#include <assimp/postprocess.h>

#include "Material.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include "SceneManager.h"
#include "Texture.h"


namespace BiBuild {
    struct Vertex;

    class ResourceManager {
        std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes{};
        std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> shaderPrograms{};
        std::unordered_map<std::string, std::unique_ptr<Texture>> textures{};
        std::unordered_map<std::string, std::unique_ptr<Material>> materials{};


        ResourceManager() = default;
        ~ResourceManager() = default;

        static ResourceManager& Get() {
            static ResourceManager instance;
            return instance;
        }


        std::vector<Mesh*> processAssimpScene(const struct aiScene *ai_scene, const std::string &filepath);

    public:
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        static Mesh* LoadMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        static Mesh* LoadMesh(const std::string& name, const std::vector<float>& vertices, const std::vector<unsigned int>& indices); // Overload for simple models that use only positions
        static Mesh* LoadMesh(const std::string& name, const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<unsigned int>& indices);
        static Mesh* LoadMesh(const std::string& name, const float* vertices, const float* normals, const unsigned int vSize, const unsigned short* indices, const unsigned int iSize, const glm::vec3& color);
        static Mesh* LoadMesh(const std::string &name, const float *vertices, unsigned int vSize, const unsigned short *indices, unsigned int iSize);
        static Mesh* LoadMesh(const std::string &name, const float *vertices, unsigned int vSize, const unsigned short *indices, unsigned int iSize, const glm::vec3& color);
        static Mesh* LoadMesh(const std::string& name, const std::string& filepath);

        static Mesh* GetMesh(const std::string& filepath);
        static Mesh* GetMesh(const std::string &name, int idx);

        static Mesh* DeleteMesh(const std::string& name, int idx);

        static std::vector<Mesh *> LoadMeshesFromFile(const std::string &filepath);

        static ShaderProgram* LoadShaderProgram(const std::string& name, const std::string& vertexFilepath, const std::string& fragmentFilepath);
        static ShaderProgram* GetShaderProgram(const std::string& name);

        static Texture* LoadTexture(const std::string& path);
        static Texture* GetTexture(const std::string& path);

        static Material* CreateMaterial(const std::string& name);
        static Material* GetMaterial(const std::string &name);

    };
}




#endif //VIEWER_RESOURSEMANAGER_H
