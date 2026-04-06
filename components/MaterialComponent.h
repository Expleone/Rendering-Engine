//
// Created by expleoene on 4/2/26.
//

#ifndef VIEWER_MATERIAL_H
#define VIEWER_MATERIAL_H
#include "Component.h"
#include "glm/vec3.hpp"
#include "glm/detail/func_packing_simd.inl"
#include "../core/ShaderProgram.h"

namespace BiBuild {

    class MaterialComponent : public Component {
    public:
        MaterialComponent(SceneObject* owner) : Component(owner) {}
        ShaderProgram* shader = nullptr;
        glm::vec3 ambient = glm::vec3(0.2f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(0.5f);
        glm::vec3 emission = glm::vec3(0.0f);
        float shininess = 32.0f;

        void SendToShader(ShaderProgram* shader_program) const;
    };

} // BiBuild

#endif //VIEWER_MATERIAL_H
