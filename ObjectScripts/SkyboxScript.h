//
// Created by expleoene on 4/20/26.
//

#ifndef VIEWER_SKYBOXSCRIPT_H
#define VIEWER_SKYBOXSCRIPT_H
#include "ObjectScript.h"
#include "glm/vec3.hpp"
#include "../components/LightComponent.h"
#include "../components/ModelComponent.h"
#include "../core/SceneObject.h"
#include "GLFW/glfw3.h"
#include "../setup.h"

namespace holubiho {

class SkyboxScript : public ObjectScript {
    double startTime;
    double prevTime{};
    LightComponent* sunlight;


public:
    glm::vec3 sunPos{};
    Texture *dayTexture = nullptr;
    Texture *nightTexture = nullptr;

    SkyboxScript(SceneObject* owner) : ObjectScript(owner) {
        startTime = glfwGetTime();
        sunlight = owner->GetComponent<LightComponent>();
        auto model = owner->GetComponent<ModelComponent>();
        if (model && model->mat && model->mat->shader)
            model->mat->AddInfo("sunPos", &sunPos, UniformType::Vec3);
    }
    void Update() override;
};

} // BiBuild

#endif //VIEWER_SKYBOXSCRIPT_H
