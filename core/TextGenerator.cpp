//
// Created by expleoene on 5/4/26.
//

#include "TextGenerator.h"

#include "../components/ModelComponent.h"
#include "../ObjectScripts/TextScript.h"

namespace BiBuild {
    class TextScript;

    void TextGenerator::CreateText(SceneObject *obj, std::string &text, glm::vec3 color) {
        auto script = obj->AddScript<TextScript>();
        script->positions.reserve(text.length());
        float startX = 0.0f; // Save the initial X position for carriage returns
        float x = startX;
        float y = 0.0f;
        int i = 0;

        for (auto c : text) {
            if (c == '\n') {
                x = startX;
                y -= Get().lineSpacing;
                continue;
            }

            Character ch = Get().characters[c];

            float xpos = x + ch.bearing.x;
            float ypos = y + ch.bearing.y;

            auto pos = glm::vec2(xpos, ypos);

            auto model = obj->AddComponent<ModelComponent>();
            model->mesh = ch.mesh;
            model->mat = ResourceManager::CreateMaterial(uuids::to_string(obj->uuid)+":"+text+":"+std::to_string(i));
            model->mat->textures.push_back(ch.tex);
            model->mat->shader = ResourceManager::LoadShaderProgram("text_shader", "./shaders/vertex/text.vert", "./shaders/fragment/text.frag");
            model->mat->diffuse = glm::vec4(color,1);
            script->positions.push_back(pos);
            model->mat->AddInfo("symbolPos", &script->positions[i], UniformType::Vec2);


            x += ch.advance;
            i++;
        }
    }
    void TextGenerator::CreateText(SceneObject *obj, std::string &text, Texture* tex, float uvScale) {
        auto script = obj->AddScript<TextScript>();
        script->positions.reserve(text.length());
        script->uvScale = uvScale;
        float startX = 0.0f; // Save the initial X position for carriage returns
        float x = startX;
        float y = 0.0f;
        int i = 0;

        float totalWidth = 0;
        float maxHeight = 0;
        float minHeight = 0;
        float currentX = 0;
        float currentY = 0;

        std::vector<ModelComponent*> models;

        for (auto c : text) {
            if (c == '\n') {
                x = startX;
                y -= Get().lineSpacing;
                continue;
            }

            Character ch = Get().characters[c];

            float xpos = x + ch.bearing.x;
            float ypos = y + ch.bearing.y;

            auto pos = glm::vec2(xpos, ypos);

            auto model = obj->AddComponent<ModelComponent>();
            models.push_back(model);
            model->mesh = ch.mesh;
            model->mat = ResourceManager::CreateMaterial(uuids::to_string(obj->uuid)+":"+text+":"+std::to_string(i));
            model->mat->textures.push_back(ch.tex);
            model->mat->textures.push_back(tex);
            model->mat->shader = ResourceManager::LoadShaderProgram("text_shader", "./shaders/vertex/text.vert", "./shaders/fragment/text.frag");
            script->positions.push_back(pos);
            model->mat->AddInfo("symbolPos", &script->positions[i], UniformType::Vec2);
            model->mat->AddInfo("uvScale", &script->uvScale, UniformType::Float);

            float top = currentY + ch.bearing.y;
            float bottom = top - ch.size.y;
            if (top > maxHeight) maxHeight = top;
            if (bottom < minHeight) minHeight = bottom;
            x += ch.advance;
            if (x > totalWidth) totalWidth = x;
            i++;
        }
        float totalHeight = maxHeight - minHeight;
        script->totalTextSize = glm::vec2(totalWidth, totalHeight);
        script->maxHeight = maxHeight;

        for (auto model : models) {
            model->mat->AddInfo("totalTextSize", &script->totalTextSize, UniformType::Vec2);
            model->mat->AddInfo("textOriginY", &script->maxHeight, UniformType::Float);
        }

    }
} // BiBuild