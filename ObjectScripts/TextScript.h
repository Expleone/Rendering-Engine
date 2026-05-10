//
// Created by expleoene on 5/4/26.
//

#ifndef VIEWER_TEXTSCRIPT_H
#define VIEWER_TEXTSCRIPT_H
#include <vector>

#include "ObjectScript.h"
#include "../components/ModelComponent.h"
#include "glm/vec2.hpp"

namespace BiBuild {
    class TextScript : public ObjectScript{
    public:
        TextScript(SceneObject* owner) : ObjectScript(owner){}
        glm::vec2 totalTextSize;
        float uvScale = 1;
        ModelComponent* textModel;

    };
} // BiBuild

#endif //VIEWER_TEXTSCRIPT_H
