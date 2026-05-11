//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_TRANSFORMCOMPONENT_H
#define VIEWER_TRANSFORMCOMPONENT_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp> // Required for quaternions
#include <vector>

#include "Component.h"
namespace holubiho {

    /// @brief Component that defines the position, rotation, and scale of a SceneObject in the scene. Contains properties for local position, rotation (as a quaternion), scale, and the resulting world transformation matrix. Also manages parent-child relationships between SceneObjects for hierarchical transformations.
    class TransformComponent : public Component {
    public:
        enum class TransformSpace {
            Local,
            Parent
        };
        glm::vec3 localPosition{0, 0, 0};
        // Initialize with identity quaternion (w=1, x=0, y=0, z=0)
        glm::quat localRotation{1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 localScale{1.0f, 1.0f, 1.0f};
        glm::mat4 worldMatrix{0};

        bool inheritScale = true;
        bool updateWorldMatrix = true;
        SceneObject* parent = nullptr;
        std::vector<SceneObject*> children;

        TransformComponent(SceneObject* owner) : Component(owner){}

        [[nodiscard]] glm::mat4 GetLocalMatrix() const {
            glm::mat4 m = glm::translate(glm::mat4(1.0f), localPosition);
            // Convert quaternion to a 4x4 rotation matrix and multiply
            m *= glm::mat4_cast(localRotation);
            m = glm::scale(m, localScale);
            return m;
        }

        /// @brief Rotates the transform by a given quaternion. The rotation can be applied in either local space (relative to the current orientation) or parent space (relative to the parent's orientation). The resulting rotation is normalized to prevent drift over time.
        void Rotate(const glm::quat& additionalRotation, TransformSpace space = TransformSpace::Parent) {
            if (space == TransformSpace::Parent) {
                localRotation = additionalRotation * localRotation;
            } else {
                localRotation = localRotation * additionalRotation;
            }
            localRotation = glm::normalize(localRotation);
        }

        /// @brief Returns the forward direction vector in local space of the transform based on its local rotation.
        glm::vec3 Forward() {
            return localRotation*glm::vec3(0,0,-1);
        }
        /// @brief Returns the right direction vector in local space of the transform based on its local rotation.
        glm::vec3 Up() {
            return localRotation*glm::vec3(0,1,0);
        }

    };
}


#endif //VIEWER_TRANSFORMCOMPONENT_H
