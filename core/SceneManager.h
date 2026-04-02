//
// Created by expleoene on 3/27/26.
//

#ifndef VIEWER_SCENEMANAGER_H
#define VIEWER_SCENEMANAGER_H
#include <memory>
#include <vector>
#include "SceneObject.h"
#include "../components/TransformComponent.h"
#include "../helper_functions/helper.h"

namespace BiBuild {

    class SceneManager {
    private:

        SceneObject* rootObject = nullptr;


    public:
        std::vector<std::unique_ptr<SceneObject>> objects;
        SceneManager();
        SceneObject* cameraObject = nullptr;
        [[nodiscard]] SceneObject* GetRoot() const;

        SceneObject* CreateObject(const std::string& name);

        void UpdateScene();

        static void UpdateTransformHierarchy(TransformComponent* transform, const glm::mat4& parentWorldMatrix);
    };
} // BiBuild

#endif //VIEWER_SCENEMANAGER_H