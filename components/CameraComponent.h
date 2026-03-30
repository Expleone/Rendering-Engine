//
// Created by expleoene on 3/30/26.
//

#ifndef VIEWER_CAMERACOMPONENT_H
#define VIEWER_CAMERACOMPONENT_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Component.h"

namespace BiBuild {
    enum class ProjectionType{Perspective, Orthographic};

    class CameraComponent : public Component{
    public:
        float fov = 60;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        float aspectRatio = 16.0/9.0;
        ProjectionType projection = ProjectionType::Perspective;

        CameraComponent(SceneObject* owner);

        static glm::mat4 FastCameraInverse(const glm::mat4& worldMatrix);

        glm::mat4 GetProjectionMat();
        glm::mat4 GetViewMat();
    };
}




#endif //VIEWER_CAMERACOMPONENT_H
