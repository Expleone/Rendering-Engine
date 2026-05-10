//
// Created by expleoene on 5/4/26.
//

#ifndef VIEWER_TEXTGENERATOR_H
#define VIEWER_TEXTGENERATOR_H

#include <ft2build.h>
#include <iostream>
#include <map>

#include "ResourceManager.h"
#include "Texture.h"
#include "glm/vec2.hpp"

#include FT_FREETYPE_H

#define BB_FONT_HEIGHT 200
#define BB_ATLAS_PADDING 5
#define MAX_ATLAS_SIZE 8192 // Must be at least the sum of widths of all characters plus padding

namespace BiBuild {
    class TextScript;
    class ModelComponent;

    class TextGenerator {
        struct Character {
            glm::ivec2   size;       // Size of glyph
            glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
            std::vector<Vertex> baseMesh;
            long advance;    // Offset to advance to next glyph
        };


        FT_Library ft;
        FT_Face face;
        Texture* atlas;
        int atlasWidth = 0;
        int atlasHeight = 0;
        std::map<char, Character> characters{};
        unsigned int lineSpacing;
        unsigned int ascender;

        TextGenerator() = default;
        ~TextGenerator() = default;
        static TextGenerator& Get() {
            static TextGenerator it;
            return it;
        }
    public:
        TextGenerator(const TextGenerator&) = delete;
        TextGenerator& operator=(const TextGenerator&) = delete;


        static void Init(const char* path) {
            if (FT_Init_FreeType(&Get().ft))
            {
                std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
                exit(0);
            }

            if (FT_New_Face(Get().ft, path, 0, &Get().face))
            {
                std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
                exit(0);
            }
            FT_Set_Pixel_Sizes(Get().face, 0, BB_FONT_HEIGHT);

            // for (unsigned char c = 0; c < 128; c++) {
            //     FT_Load_Char(Get().face, c, FT_LOAD_RENDER);
            //     Get().atlasWidth += Get().face->glyph->bitmap.width + BB_ATLAS_PADDING;
            //     if (Get().face->glyph->bitmap.rows > Get().atlasHeight) {
            //         Get().atlasHeight = Get().face->glyph->bitmap.rows;
            //     }
            // }

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
            Get().atlas = ResourceManager::CreateTexture("Atlas", TexType::Tex2D, MAX_ATLAS_SIZE, MAX_ATLAS_SIZE, false, GL_RED);
            int xoffset = 0;
            int yoffset = 0;
            Get().lineSpacing  = Get().face->size->metrics.height >> 6;
            Get().ascender     = Get().face->size->metrics.ascender >> 6;

            for (unsigned char c = 0; c < 128; c++)
            {
                if (FT_Load_Char(Get().face, c, FT_LOAD_RENDER))
                {
                    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                    continue;
                }
                if (xoffset + Get().face->glyph->bitmap.width > MAX_ATLAS_SIZE) {
                    yoffset += Get().lineSpacing + BB_ATLAS_PADDING;
                    xoffset = 0;
                }

                Get().atlas->UpdateTexture(Get().face->glyph->bitmap.buffer, xoffset, yoffset, Get().face->glyph->bitmap.width, Get().face->glyph->bitmap.rows, GL_RED);

                float rows = static_cast<float>(Get().face->glyph->bitmap.rows);
                float width = static_cast<float>(Get().face->glyph->bitmap.width);

                float uMin = static_cast<float>(xoffset) / static_cast<float>(MAX_ATLAS_SIZE);
                float uMax = static_cast<float>(xoffset + width) / static_cast<float>(MAX_ATLAS_SIZE);
                float vMin = yoffset / static_cast<float>(MAX_ATLAS_SIZE);
                float vMax = (yoffset + rows) / static_cast<float>(MAX_ATLAS_SIZE);

                std::vector<Vertex> vertices = {
                    { // Bottom-Left
                        glm::vec3(0, -rows, 0),
                        glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0),
                        glm::vec2(uMin, vMax),
                        glm::vec3(0)
                    },
                    { // Top-Left
                        glm::vec3(0, 0, 0),
                        glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0),
                        glm::vec2(uMin, vMin),
                        glm::vec3(0)
                    },
                    { // Top-Right
                        glm::vec3(width, 0, 0),
                        glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0),
                        glm::vec2(uMax, vMin),
                        glm::vec3(0)
                    },
                    { // Bottom-Right
                        glm::vec3(width, -rows, 0),
                        glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0),
                        glm::vec2(uMax, vMax),
                        glm::vec3(0)
                    },
                };

                Character character = {
                    glm::ivec2(Get().face->glyph->bitmap.width, Get().face->glyph->bitmap.rows),
                    glm::ivec2(Get().face->glyph->bitmap_left, Get().face->glyph->bitmap_top),
                    vertices,
                    Get().face->glyph->advance.x >> 6
                };

                Get().characters.insert(std::pair<char, Character>(c, character));

                xoffset += Get().face->glyph->bitmap.width + BB_ATLAS_PADDING;
            }



            FT_Done_Face(Get().face);
            FT_Done_FreeType(Get().ft);
        }

        static TextScript* CreateText(SceneObject *obj, std::string text, float scale = 1, glm::vec3 color = {1, 1, 1});
        static Mesh* CreateTextMesh(std::string &text, float scale, glm::vec2* totalTextSize = nullptr);

    };
} // BiBuild

#endif //VIEWER_TEXTGENERATOR_H
