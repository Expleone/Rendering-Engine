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
        glm::vec3 ambient = glm::vec3(0.2f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(0.5f);
        glm::vec3 emission = glm::vec3(0.0f);
        float shininess = 32.0f;

        // 1. Diffuse texture, 2. Specular texture, 3. Normal map, 4. Environment map, 5-16 are reserved for future use
        std::vector<Texture*> textures;

        void SendToShader(ShaderProgram* shaderProgram) const;
    };

} // BiBuild

#endif //VIEWER_CORE_MATERIAL_H
