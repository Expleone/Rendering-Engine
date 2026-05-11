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


namespace holubiho {
    struct Vertex;

    /// @brief Singleton class responsible for managing all resources in the application, including meshes, shader programs, textures, and materials.
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


        std::vector<Mesh*> processAssimpSceneMeshes(const struct aiScene *ai_scene, const std::string &filepath);

        void processAssimpSceneObjects(const struct aiScene *ai_scene, const std::string &filepath,
                                       SceneObject *parent, bool asSeparateObjects = false);

    public:
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        /// @brief Loads a mesh with the given name, vertices, and indices. If a mesh with the same name already exists, it updates the existing mesh's vertex data instead of creating a new one.
        /// @param name The unique name of the mesh to load or update.
        /// @param vertices A vector of Vertex structures containing the vertex data for the mesh.
        /// @param indices A vector of unsigned integers representing the index data for the mesh.
        static Mesh* LoadMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

        /// @brief Retrieves a mesh by its unique name. If the mesh does not exist, it tries to load first mesh from the file with given name, if it does not succeed returns nullptr.
        /// @param name The unique name of the mesh to retrieve.
        static Mesh* GetMesh(const std::string& name);

        /// @brief Retrieves a mesh by its unique name. If the mesh does not exist, it tries to load mesh with given index from the file with given name, if it does not succeed returns nullptr.
        /// @param name The unique name of the mesh to retrieve.
        /// @param idx The index of the mesh to retrieve from the file if the mesh with given name does not exist.
        static Mesh* GetMesh(const std::string &name, int idx);

        /// @brief Loads meshes from a file using the Assimp library. The method processes the file and creates Mesh objects for each mesh found in the file, storing them in the resource manager for future retrieval.
        /// @param filepath The path to the file from which to load the meshes.
        static std::vector<Mesh *> LoadMeshesFromFile(const std::string &filepath);

        /// @brief Loads models from a file using the Assimp library. The method processes the file and creates SceneObject instances for each mesh found in the file, attaching them to the specified parent object in the scene hierarchy, if asSeparateObjects has been set to true.
        /// @param filepath The path to the file from which to load the models.
        /// @param parent The parent SceneObject to which the loaded models will be attached in the scene hierarchy.
        /// @param asSeparateObjects If true, each mesh in the file will be loaded as a separate SceneObject attached to the parent; if false, all meshes will be combined into one ModelComponent attached to the parent SceneObject.
        static void LoadModelsFromFile(const std::string &filepath, SceneObject *parent,
                                               bool asSeparateObjects);
        /// @brief Loads a shader program from vertex and fragment shader source files. If a shader program with the same name already exists, it returns the existing one instead of creating a new one.
        /// @param name The unique name of the shader program to load or retrieve.
        /// @param vertexFilepath The file path to the vertex shader source code.
        /// @param fragmentFilepath The file path to the fragment shader source code.
        static ShaderProgram* LoadShaderProgram(const std::string& name, const std::string& vertexFilepath, const std::string& fragmentFilepath);

        /// @brief Retrieves a shader program by its unique name. If the shader program does not exist, it returns nullptr.
        /// @param name The unique name of the shader program to retrieve.
        static ShaderProgram* GetShaderProgram(const std::string& name);

        /// @brief Loads a texture from a file.
        /// @param path The file path to the texture image.
        static Texture* LoadTexture(const std::string& path);

        /// @brief Loads a texture from a file with specified wrapping and filtering modes.
        /// @param path The file path to the texture image.
        /// @param wrapMode The OpenGL wrapping mode to use for the texture (e.g., GL_REPEAT, GL_CLAMP_TO_EDGE).
        /// @param filterMode The OpenGL filtering mode to use for the texture (e.g., GL_LINEAR, GL_NEAREST).
        static Texture* LoadTexture(const std::string& path, GLint wrapMode, GLint filterMode);

        /// @brief Creates a new texture with the specified parameters.
        /// @param name The unique name of the texture to create.
        /// @param width The width of the texture in pixels.
        /// @param height The height of the texture in pixels.
        /// @param mipmap Whether to generate mipmaps for the texture.
        static Texture* CreateTexture(const std::string& name, int width, int height, bool mipmap,  GLuint format);

        /// @brief Loads a cube map texture from a set of image files representing the faces of the cube. The method checks if a cube map with the same set of faces already exists and returns it if found; otherwise, it creates a new cube map texture and stores it in the resource manager.
        /// @param faces A vector of file paths to the images for each face of the cube map, typically in the order: right, left, top, bottom, front, back.
        static Texture* LoadTextureCubeMap(const std::vector<std::string> &faces);

        /// @brief Retrieves a texture by its unique name. If the texture does not exist tries to load texture from given path, if it does not succeed it returns nullptr.
        /// @param path The unique name or file path of the texture to retrieve.
        static Texture* GetTexture(const std::string& path);

        /// @brief Creates a new material with the specified name. If a material with the same name already exists, it returns the existing one instead of creating a new one.
        /// @param name The unique name of the material to create or retrieve.
        static Material* CreateMaterial(const std::string& name);

        /// @brief Retrieves a material by its unique name. If the material does not exist, it returns nullptr.
        /// @param name The unique name of the material to retrieve.
        static Material* GetMaterial(const std::string &name);

    };
}




#endif //VIEWER_RESOURSEMANAGER_H
