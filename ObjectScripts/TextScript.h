//
// Created by expleoene on 5/4/26.
//

#ifndef VIEWER_TEXTSCRIPT_H
#define VIEWER_TEXTSCRIPT_H
#include <vector>

#include "ObjectScript.h"
#include "glm/vec2.hpp"

namespace BiBuild {
    class TextScript : public ObjectScript{
    public:
        TextScript(SceneObject* owner) : ObjectScript(owner){}
        std::vector<glm::vec2> positions;
        glm::vec2 totalTextSize;
        float maxHeight;
        float uvScale;

    };
} // BiBuild

#endif //VIEWER_TEXTSCRIPT_H
