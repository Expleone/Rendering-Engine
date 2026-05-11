//
// Created by expleoene on 4/13/26.
//

#ifndef VIEWER_CORE_MATERIAL_H
#define VIEWER_CORE_MATERIAL_H
#include "ShaderProgram.h"
#include "Texture.h"

namespace holubiho {


class Material {
    public:



        Material(){}
        ShaderProgram* shader = nullptr;
        glm::vec3 ambient = glm::vec4(0.2f);
        glm::vec4 diffuse = glm::vec4(1.0f);
        glm::vec3 specular = glm::vec3(0.5f);
        glm::vec3 emission = glm::vec3(0.0f);
        float shininess = 64.0f;
        std::vector<AdditionalShaderInfo> shaderInfo;

        // 1. Diffuse texture, 2. Normal map
        std::vector<Texture*> textures;

        /// @brief Sends the material properties and textures to the shader program.
        /// @param shaderProgram The shader program to which the material properties will be sent.
        void SendToShader(ShaderProgram *shaderProgram) const;

        /// @brief Sends additional shader information that is not defined by typical material.
        /// @param shaderProgram The shader program to which the additional information will be sent.
        void SendAdditionalInfo(ShaderProgram *shaderProgram) const {
            for (auto& info : shaderInfo) {
                if (!info.infoPtr) {
                    continue;
                }

                switch (info.type) {
                    case UniformType::Integer:
                        shaderProgram->SetUniformInt(info.uniformName.c_str(), *static_cast<const int*>(info.infoPtr));
                        break;
                    case UniformType::Vec3:
                        shaderProgram->SetUniformVec3(info.uniformName.c_str(), *static_cast<const glm::vec3*>(info.infoPtr));
                        break;
                    case UniformType::Vec2:
                        shaderProgram->SetUniformVec2(info.uniformName.c_str(), *static_cast<const glm::vec2*>(info.infoPtr));
                        break;
                    case UniformType::Float:
                        shaderProgram->SetUniformFloat(info.uniformName.c_str(), *static_cast<const float*>(info.infoPtr));
                        break;
                }
            }
        }

        /// @brief Adds additional shader information to be sent to the shader program.
        /// @param name The name of the uniform variable in the shader program that will receive the additional information. This should match the name defined in the shader code.
        /// @param ptr A pointer to the data that will be sent to the shader program.
        /// @param type The type of the uniform variable (e.g., Integer, Vec3, Vec2, Float) that indicates how the data should be interpreted and sent to the shader program.
        void AddInfo(const std::string& name, const void* ptr, const UniformType type) {
            shaderInfo.push_back({name, ptr, type});
        }
    };



} // BiBuild

#endif //VIEWER_CORE_MATERIAL_H
