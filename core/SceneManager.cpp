//
// Created by expleoene on 3/27/26.
//

#include "SceneManager.h"

#include "../components/CameraComponent.h"

namespace BiBuild {

    SceneManager::SceneManager() {
        // Initialize the hidden root object
        rootObject = CreateObject("SceneRoot");
        cameraObject = CreateObject("MainCamera");
        cameraObject->AddComponent<CameraComponent>();
    }

    SceneObject* SceneManager::GetRoot() const {
        return rootObject;
    }

    SceneObject*  SceneManager::CreateObject(const std::string& name) {
        auto uuid = Helper::genUUID();
        auto obj = std::make_unique<SceneObject>(uuid, name);

        SceneObject* ptr = obj.get();
        objects.push_back(std::move(obj));///Crushes here sometimes

        // Automatically parent to root, unless this IS the root
        if (rootObject != nullptr) {
            rootObject->AddChild(ptr);
        }

        return ptr;
    }

    void SceneManager::UpdateScene() {
        if (rootObject) {
            UpdateTransformHierarchy(rootObject->GetComponent<TransformComponent>(), glm::mat4(1));
        }
    }

    void SceneManager::UpdateTransformHierarchy(TransformComponent* transform, const glm::mat4& parentWorldMatrix) {
        glm::mat4 effectiveParentMatrix = parentWorldMatrix;

        if (!transform->inheritScale) {
            glm::vec3 right = glm::normalize(glm::vec3(parentWorldMatrix[0]));
            glm::vec3 up    = glm::normalize(glm::vec3(parentWorldMatrix[1]));
            glm::vec3 fwd   = glm::normalize(glm::vec3(parentWorldMatrix[2]));

            glm::vec3 pos   = glm::vec3(parentWorldMatrix[3]);

            effectiveParentMatrix = glm::mat4(
                glm::vec4(right, 0.0f),
                glm::vec4(up,    0.0f),
                glm::vec4(fwd,   0.0f),
                glm::vec4(pos,   1.0f)
            );
        }

        transform->worldMatrix = effectiveParentMatrix * transform->GetLocalMatrix();

        for (SceneObject* child : transform->children) {
            if (TransformComponent* childTransform = child->GetComponent<TransformComponent>()) {
                UpdateTransformHierarchy(childTransform, transform->worldMatrix);
            }
        }
    }

} // BiBuild