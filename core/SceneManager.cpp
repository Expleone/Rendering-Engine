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
        objects.push_back(std::move(obj));

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

        // The Fix: Strip scaling from the parent's matrix if the flag is false
        if (!transform->inheritScale) {
            // Extract and normalize the X, Y, and Z axis vectors
            glm::vec3 right = glm::normalize(glm::vec3(parentWorldMatrix[0]));
            glm::vec3 up    = glm::normalize(glm::vec3(parentWorldMatrix[1]));
            glm::vec3 fwd   = glm::normalize(glm::vec3(parentWorldMatrix[2]));

            // Extract the translation (position remains unaffected)
            glm::vec3 pos   = glm::vec3(parentWorldMatrix[3]);

            // Rebuild a pure rotation/translation matrix
            effectiveParentMatrix = glm::mat4(
                glm::vec4(right, 0.0f),
                glm::vec4(up,    0.0f),
                glm::vec4(fwd,   0.0f),
                glm::vec4(pos,   1.0f)
            );
        }

        // Calculate final world matrix
        transform->worldMatrix = effectiveParentMatrix * transform->GetLocalMatrix();

        // Recursively update all children
        for (SceneObject* child : transform->children) {
            if (TransformComponent* childTransform = child->GetComponent<TransformComponent>()) {
                UpdateTransformHierarchy(childTransform, transform->worldMatrix);
            }
        }
    }

} // BiBuild