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
#include "GLFW/glfw3.h"

namespace BiBuild {

    enum class ProjectionType{Perspective, Orthographic};

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

        glm::mat4 FastCameraInverse();

        glm::mat4 GetProjectionMat();
        glm::mat4 GetViewMat();

        glm::mat4 BuildCameraViewMatrix() const;
    };
}

#endif //VIEWER_CAMERACOMPONENT_H