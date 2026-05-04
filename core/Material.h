//
// Created by expleoene on 4/13/26.
//

#ifndef VIEWER_CORE_MATERIAL_H
#define VIEWER_CORE_MATERIAL_H
#include "ShaderProgram.h"
#include "Texture.h"

namespace BiBuild {


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

        // 1. Diffuse texture, 2. Normal map, 3. Environment map, 4-16 are reserved for future use
        std::vector<Texture*> textures;

        void SendToShader(ShaderProgram *shaderProgram) const;

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

        void AddInfo(const std::string& name, const void* ptr, const UniformType type) {
            shaderInfo.push_back({name, ptr, type});
        }
    };



} // BiBuild

#endif //VIEWER_CORE_MATERIAL_H
