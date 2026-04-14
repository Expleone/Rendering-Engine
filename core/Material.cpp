//
// Created by expleoene on 4/13/26.
//

#include "Material.h"

#include "RenderSystem.h"

namespace BiBuild {
    void Material::SendToShader(ShaderProgram* shaderProgram) const {

        
        
        if (shaderProgram) {
            shaderProgram->Use();
            shaderProgram->SetUniformVec3("material.ambient", ambient);
            shaderProgram->SetUniformVec3("material.diffuse", diffuse);
            shaderProgram->SetUniformVec3("material.specular", specular);
            shaderProgram->SetUniformVec3("material.emission", emission);
            shaderProgram->SetUniformFloat("material.shininess", shininess);
        }


        for (size_t i = 0; i < textures.size(); ++i) {
            if (textures[i]) {
                glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(i));
                glBindTexture(GL_TEXTURE_2D, textures[i]->GetID());
                shaderProgram->SetUniformInt(("material.textures[" + std::to_string(i) + "]").c_str(), static_cast<int>(i));
            }
        }
    }
} // BiBuild