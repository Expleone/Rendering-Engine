//
// Created by expleoene on 5/4/26.
//

#include "TextGenerator.h"

#include "../components/ModelComponent.h"
#include "../ObjectScripts/TextScript.h"

namespace BiBuild {
    class TextScript;

    TextScript* TextGenerator::CreateText(SceneObject *obj, std::string text, float scale, glm::vec3 color) {
        auto script = obj->AddScript<TextScript>();
        auto model = obj->AddComponent<ModelComponent>();
        script->textModel = model;
        model->mesh = CreateTextMesh(text, scale, &script->totalTextSize);
        std::hash<std::string> hasher;
        size_t hashVal = hasher(text);
        model->mat = ResourceManager::CreateMaterial(uuids::to_string(obj->uuid) + "_text_" + std::to_string(hashVal));
        model->mat->textures.push_back(Get().atlas);
        model->mat->ambient = glm::vec4(color, 1.0f);
        model->mat->diffuse = glm::vec4(color, 1.0f);
        model->mat->shader = ResourceManager::LoadShaderProgram("text_shader", "./shaders/vertex/text.vert", "./shaders/fragment/text.frag");
        model->mat->AddInfo("totalTextSize", &script->totalTextSize, UniformType::Vec2);
        model->mat->AddInfo("uvScale", &script->uvScale, UniformType::Float);
        return script;
    }

    Mesh *TextGenerator::CreateTextMesh(std::string &text, float scale, glm::vec2* totalTextSize) {
        float startX = 0.0f;
        float x = startX;
        float y = 0.0f; // This is now the TOP-LEFT of the text block

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        vertices.reserve(text.length() * 4);
        indices.reserve(text.length() * 6);

        unsigned int baseInd[6] = { 0, 2, 1, 0, 3, 2 };
        unsigned int startIdx = 0;
        float xright = 0;
        float ydown = 0;

        for (auto c : text) {
            if (Get().characters.find(c) == Get().characters.end()) {
                continue;
            }

            if (c == '\n') {
                x = startX;
                y -= (Get().lineSpacing * scale);
                continue;
            }

            Character ch = Get().characters[c];

            float baselineY = y - (Get().ascender * scale);

            float xpos = x + (ch.bearing.x * scale);
            float ypos = baselineY + (ch.bearing.y * scale);

            for (auto vertex : ch.baseMesh) {
                vertex.position = (vertex.position * scale) + glm::vec3(xpos, ypos, 0);
                vertices.push_back(vertex);
                if (vertex.position.x > xright) xright = vertex.position.x;
                if (vertex.position.y < ydown) ydown = vertex.position.y;
            }

            for (auto idx : baseInd) {
                indices.push_back(idx + startIdx);
            }

            startIdx += 4;
            x += (ch.advance * scale);
        }

        if (totalTextSize) *totalTextSize = glm::vec2(xright, -ydown);

        std::hash<std::string> hasher;
        size_t hashVal = hasher(text);
        std::string batchName = "text_mesh_" + std::to_string(hashVal) + "_s" + std::to_string(scale);
        return ResourceManager::LoadMesh(batchName, vertices, indices);
    }
} // BiBuild