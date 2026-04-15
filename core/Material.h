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
        glm::vec4 ambient = glm::vec4(0.2, 0.2, 0.2, 1);
        glm::vec4 diffuse = glm::vec4(1.0f);
        glm::vec3 specular = glm::vec3(0.5f);
        glm::vec3 emission = glm::vec3(0.0f);
        float shininess = 64.0f;

        // 1. Diffuse texture, 2. Normal map, 3. Environment map, 4-16 are reserved for future use
        std::vector<Texture*> textures;

        void SendToShader(ShaderProgram* shaderProgram) const;
    };

} // BiBuild

#endif //VIEWER_CORE_MATERIAL_H
