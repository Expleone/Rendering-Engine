//
// Created by expleoene on 3/27/26.
//

#ifndef VIEWER_SCENEMANAGER_H
#define VIEWER_SCENEMANAGER_H
#include <map>
#include <memory>
#include <vector>
#include "SceneObject.h"
#include "../components/TransformComponent.h"
#include "../helper_functions/helper.h"


namespace holubiho {

    class SceneManager {
    private:

        SceneObject* rootObject = nullptr;
        float fogDistClose = 40;
        float fogDistFar = 60;

    public:
        std::vector<std::unique_ptr<SceneObject>> objects;
        std::unordered_map<uuids::uuid, SceneObject*> objects_by_ids;
        SceneManager();
        SceneObject* cameraObject = nullptr;
        SceneObject* skybox = nullptr;

        /// @brief Returns a pointer to the root SceneObject of the scene
        [[nodiscard]] SceneObject* GetRoot() const;

        /// @brief Creates a new SceneObject with the given name and adds it to the scene. Returns a pointer to the newly created SceneObject.
        /// @param name The name to assign to the new SceneObject.
        SceneObject* CreateObject(const std::string& name);

        /// @brief Retrieves a SceneObject by its unique UUID. Returns a pointer to the SceneObject if found, or nullptr if no object with the given UUID exists in the scene.
        /// @param id The unique UUID of the SceneObject to retrieve.
        SceneObject* GetObject(const uuids::uuid id);

        /// @brief Creates a skybox SceneObject using the provided cube map textures for day and night. If a skybox already exists, it returns the existing one instead of creating a new one.
        /// @param faces A vector of file paths to the images for each face of the cube map texture for day.
        /// @param facesNight A vector of file paths to the images for each face of the cube map texture for night.
        SceneObject* CreateSkyBox(const std::vector<std::string> &faces, const std::vector<std::string> &facesNight);

        /// @brief Changes the textures of the existing skybox to new cube map textures for day and night. If no skybox exists, it creates a new one using the provided textures.
        /// @param faces A vector of file paths to the images for each face of the new cube map texture for day.
        /// @param facesNight A vector of file paths to the images for each face of the new cube map texture for night.
        void ChangeSkyBox(const std::vector<std::string> &faces, const std::vector<std::string> &facesNight);

        /// @brief Applies initial settings to the scene, such as configuring the render system's fog parameters based on the current fog distance settings.
        void ApplySceneSettings();

        /// @brief Sets the distances for fog effects in the scene. The 'close' parameter defines the distance at which the fog starts to affect objects, while the 'far' parameter defines the distance at which objects are fully obscured by fog.
        /// @param close The distance at which the fog starts to affect objects in the scene.
        /// @param far The distance at which objects are fully obscured by fog in the scene
        void SetFogDistance(float close, float far);

        /// @brief Updates the scene by first updating all scripts attached to SceneObjects and then recursively updating the world transformation matrices of all SceneObjects in the hierarchy starting from the root object. This method should be called once per frame to ensure that all transformations and script logic are up to date before rendering.
        void UpdateScene();

        private:
        /// @brief Recursively updates the world transformation matrix of a given TransformComponent and all of its child TransformComponents in the scene hierarchy.
        /// @param transform The TransformComponent to update, along with its children.
        /// @param parentWorldMatrix The world transformation matrix of the parent TransformComponent, which is used to calculate the world matrix of the current TransformComponent.
        static void UpdateTransformHierarchy(TransformComponent* transform, const glm::mat4& parentWorldMatrix);

        /// @brief Iterates through all SceneObjects in the scene and calls their ExecuteScripts method to update any attached scripts.
        void UpdateScripts();
    };
} // BiBuild

#endif //VIEWER_SCENEMANAGER_H