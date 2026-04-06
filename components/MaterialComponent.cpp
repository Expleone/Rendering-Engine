//
// Created by expleoene on 4/2/26.
//

#include "MaterialComponent.h"

namespace BiBuild {
        void MaterialComponent::SendToShader(ShaderProgram* shader_program) const {
            if (shader_program) {
                shader_program->Use();
                shader_program->SetUniformVec3("material.ambient", ambient);
                shader_program->SetUniformVec3("material.diffuse", diffuse);
                shader_program->SetUniformVec3("material.specular", specular);
                shader_program->SetUniformVec3("material.emission", emission);
                shader_program->SetUniformFloat("material.shininess", shininess);
            }
        }
} // BiBuild