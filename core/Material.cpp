//
// Created by expleoene on 4/13/26.
//

#include "Material.h"

#include "RenderSystem.h"

namespace holubiho {
    void Material::SendToShader(ShaderProgram* shaderProgram) const {
        if (shaderProgram) {
            shaderProgram->Use();
            SendAdditionalInfo(shaderProgram);
            shaderProgram->SetUniformVec3("material.ambient", ambient);
            shaderProgram->SetUniformVec4("material.diffuse", diffuse);
            shaderProgram->SetUniformVec3("material.specular", specular);
            shaderProgram->SetUniformVec3("material.emission", emission);
            shaderProgram->SetUniformFloat("material.shininess", shininess);
        }

        auto fogTex = RenderSystem::GetFogTexture();
        auto nightFogTex = RenderSystem::GetNightFogTexture();
        GLuint textureUnit = 0;
        int tex2DCount = 0;


        for (size_t i = 0; i < textures.size(); ++i) {
            if (textures[i]) {
                glActiveTexture(GL_TEXTURE0 + textureUnit);
                if (textures[i]->GetType() == TexType::Tex2D) {
                    glBindTexture(GL_TEXTURE_2D, textures[i]->GetID());
                    if (shaderProgram) {
                        shaderProgram->SetUniformInt(("material.textures[" + std::to_string(tex2DCount) + "]").c_str(), static_cast<int>(textureUnit));
                    }
                    tex2DCount++;
                }

                else {
                    glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i]->GetID());
                    if (shaderProgram)
                        shaderProgram->SetUniformInt("cubeMapTex", static_cast<int>(textureUnit));
                }
                textureUnit++;
            }
        }
        int fogUnitIndex = 15;
        int nightFogUnitIndex = 16;
        if (shaderProgram) {
            shaderProgram->SetUniformInt("fogTex", fogUnitIndex);
            shaderProgram->SetUniformInt("nightFogTex", nightFogUnitIndex);
            shaderProgram->SetUniformVec3("sunPos", RenderSystem::GetSunPosition());
        }
        if (fogTex) {
            glActiveTexture(GL_TEXTURE0 + fogUnitIndex);
            glBindTexture(GL_TEXTURE_CUBE_MAP, fogTex->GetID());
        }
        if (nightFogTex) {
            glActiveTexture(GL_TEXTURE0 + nightFogUnitIndex);
            glBindTexture(GL_TEXTURE_CUBE_MAP, nightFogTex->GetID());
        } else if (fogTex) {
            glActiveTexture(GL_TEXTURE0 + nightFogUnitIndex);
            glBindTexture(GL_TEXTURE_CUBE_MAP, fogTex->GetID());
        }


        if (shaderProgram)
        shaderProgram->SetUniformInt("material.texNum", tex2DCount);
    }
} // BiBuild