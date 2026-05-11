//
// Created by expleoene on 3/27/26.
//

#include "SceneManager.h"

#include "RenderSystem.h"
#include "ResourceManager.h"
#include "../components/CameraComponent.h"
#include "../components/LightComponent.h"
#include "../components/ModelComponent.h"
#include "../ObjectScripts/SkyboxScript.h"

namespace holubiho {

    SceneManager::SceneManager() {
        // Initialize the hidden root object
        rootObject = CreateObject("SceneRoot");
        cameraObject = CreateObject("MainCamera");
        cameraObject->AddComponent<CameraComponent>();
        ApplySceneSettings();
    }

    SceneObject* SceneManager::GetRoot() const {
        return rootObject;
    }

    SceneObject*  SceneManager::CreateObject(const std::string& name) {
        auto uuid = Helper::genUUID();
        auto obj = std::make_unique<SceneObject>(uuid, name, this);

        SceneObject* ptr = obj.get();
        objects.push_back(std::move(obj));
        objects_by_ids.emplace(ptr->uuid, ptr);

        if (rootObject != nullptr) {
            rootObject->AddChild(ptr);
        }

        return ptr;
    }

    SceneObject *SceneManager::GetObject(const uuids::uuid id) {
        auto it = objects_by_ids.find(id);
        if (it != objects_by_ids.end()) {
            return it->second;
        }
        return nullptr;
    }

    SceneObject* SceneManager::CreateSkyBox(const std::vector<std::string>& faces, const std::vector<std::string>& facesNight) {
        if (skybox) return skybox;

        auto uuid = Helper::genUUID();
        auto obj = std::make_unique<SceneObject>(uuid, "Skybox", (SceneManager*)this);
        auto model = obj->AddComponent<ModelComponent>();
        model->mesh = ResourceManager::GetMesh("resources/cube.glb");
        model->mat = ResourceManager::CreateMaterial("SkyboxMaterial");

        auto skyboxTex = ResourceManager::LoadTextureCubeMap(faces);
        auto nightSkyboxTex = ResourceManager::LoadTextureCubeMap(facesNight);
        if (skyboxTex->GetID() == 0) std::cout << "Warning: Couldn't load skybox" << std::endl;
        if (RenderSystem::GetUseSkyboxTexAsFog()) {
            RenderSystem::SetFogTexture(skyboxTex);
            RenderSystem::SetNightFogTexture(nightSkyboxTex);

        }
        auto sunTex = ResourceManager::LoadTexture("resources/textures/skyboxes/outrun-sunset.png", GL_CLAMP_TO_BORDER, GL_NEAREST);

        model->mat->textures.push_back(sunTex);
        model->mat->textures.push_back(nightSkyboxTex);
        model->mat->textures.push_back(skyboxTex);
        model->mat->shader = ResourceManager::LoadShaderProgram("SkyboxShader", "./shaders/vertex/skybox.vert", "./shaders/fragment/skybox.frag");

        auto sunlight = obj->AddComponent<LightComponent>();
        sunlight->type = LightType::Directional;
        sunlight->intensity = 1.0f;
        sunlight->ambient = glm::vec3(0.2, 0.2, 0.2);
        sunlight->diffuse = glm::vec3(0.9f, 0.9f, 0.8f);
        sunlight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
        sunlight->direction = glm::vec3(0, -1.0f, 0);
        auto scr = obj->AddScript<SkyboxScript>();
        scr->nightTexture = nightSkyboxTex;
        RenderSystem::SetSunPosPointer(&scr->sunPos);

        SceneObject* ptr = obj.get();
        if (rootObject != nullptr) {
            rootObject->AddChild(ptr);
        }

        objects.push_back(std::move(obj));
        skybox = ptr;
        return ptr;
    }


    void SceneManager::ChangeSkyBox(const std::vector<std::string> &faces, const std::vector<std::string> &facesNight) {
        if (!skybox) {
            CreateSkyBox(faces, facesNight);
            return;
        }
        const auto model = skybox->GetComponent<ModelComponent>();
        if (!model) return;
        auto mat = model->mat;
        if (!mat) return;
        auto newDayTexture = ResourceManager::LoadTextureCubeMap(faces);
        auto newNightTexture = ResourceManager::LoadTextureCubeMap(facesNight);

        if (mat->textures.empty()) {
            return;
        }

        if (mat->textures.size() < 3) {
            mat->textures.resize(3, nullptr);
        }

        mat->textures[1] = newNightTexture;
        mat->textures[2] = newDayTexture;

        if (RenderSystem::GetUseSkyboxTexAsFog()) RenderSystem::SetFogTexture(newDayTexture);
        RenderSystem::SetNightFogTexture(newNightTexture);
    }

    void SceneManager::UpdateScene() {
        UpdateScripts();
        if (rootObject) {
            UpdateTransformHierarchy(rootObject->GetComponent<TransformComponent>(), glm::mat4(1));
        }
    }



    void SceneManager::UpdateTransformHierarchy(TransformComponent* transform, const glm::mat4& parentWorldMatrix) {
        glm::mat4 effectiveParentMatrix = parentWorldMatrix;

        if (transform->updateWorldMatrix) {
            if (!transform->inheritScale) {
                glm::vec3 right = glm::normalize(glm::vec3(parentWorldMatrix[0]));
                glm::vec3 up    = glm::normalize(glm::vec3(parentWorldMatrix[1]));
                glm::vec3 fwd   = glm::normalize(glm::vec3(parentWorldMatrix[2]));

                auto pos = glm::vec3(parentWorldMatrix[3]);

                effectiveParentMatrix = glm::mat4(
                    glm::vec4(right, 0.0f),
                    glm::vec4(up,    0.0f),
                    glm::vec4(fwd,   0.0f),
                    glm::vec4(pos,   1.0f)
                );
            }

            transform->worldMatrix = effectiveParentMatrix * transform->GetLocalMatrix();
        }


        for (SceneObject* child : transform->children) {
            if (auto* childTransform = child->GetComponent<TransformComponent>()) {
                UpdateTransformHierarchy(childTransform, transform->worldMatrix);
            }
        }
    }

    void SceneManager::UpdateScripts() {
        for (auto& object : objects) {
            auto objPtr = object.get();
            if (objPtr) objPtr->ExecuteScripts();
        }
    }

    void SceneManager::ApplySceneSettings() {
        RenderSystem::SetFogDistance(fogDistClose, fogDistFar);
    }

    void SceneManager::SetFogDistance(float close, float far) {
        fogDistClose = close;
        fogDistFar = far;
        ApplySceneSettings();
    }
} // BiBuild