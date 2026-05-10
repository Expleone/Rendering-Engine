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


namespace BiBuild {

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
        [[nodiscard]] SceneObject* GetRoot() const;

        SceneObject* CreateObject(const std::string& name);
        SceneObject* GetObject(const uuids::uuid id);
        SceneObject* CreateSkyBox(const std::vector<std::string> &faces, const std::vector<std::string> &facesNight);
        void ChangeSkyBox(const std::vector<std::string> &faces, const std::vector<std::string> &facesNight);
        void ApplySceneSettings();
        void SetFogDistance(float close, float far);

        void UpdateScene();

        static void UpdateTransformHierarchy(TransformComponent* transform, const glm::mat4& parentWorldMatrix);
        void UpdateScripts();
    };
} // BiBuild

#endif //VIEWER_SCENEMANAGER_H