//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_CAMERACOMPONENT_H
#define VIEWER_CAMERACOMPONENT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Component.h"
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include "GLFW/glfw3.h"

namespace holubiho {

    enum class ProjectionType{Perspective, Orthographic};
    ///  @brief Component that defines a camera in the scene. Provides methods to get projection and view matrices, and handles camera control state for user input.
    class CameraComponent : public Component{
    public:
        struct CameraControlState {
            float mouseSensitivity = 0.12f;
            bool isMouseCaptured = false;
            double lastMouseX = 0.0;
            double lastMouseY = 0.0;
        } lookState;
        float movementSpeed = 10.0f;
        float fov = 60;
        float nearPlane = 0.1f;
        float farPlane = 10000000.0f;
        float aspectRatio = 16.0/9.0;
        ProjectionType projection = ProjectionType::Perspective;

        CameraComponent(SceneObject* owner);

        /// @brief Returns the projection matrix based on the current projection type and camera parameters.
        glm::mat4 GetProjectionMat();

        /// @brief Returns the view matrix calculated from the camera's world transform. Uses the inverse of the camera's world matrix to create the view matrix.
        glm::mat4 GetViewMat();


    };


}

#endif //VIEWER_CAMERACOMPONENT_H