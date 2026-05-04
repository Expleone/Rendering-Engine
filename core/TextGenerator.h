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

namespace BiBuild {
    class TextGenerator {
        struct Character {
            Texture* tex;  // ID handle of the glyph texture
            glm::ivec2   size;       // Size of glyph
            glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
            Mesh* mesh;
            long advance;    // Offset to advance to next glyph
        };

        FT_Library ft;
        FT_Face face;
        std::map<char, Character> characters{};
        unsigned int lineSpacing;

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

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
            Get().lineSpacing  = Get().face->size->metrics.height >> 6;
            for (unsigned char c = 0; c < 128; c++)
            {
                std::string name("glyph:" + std::to_string(static_cast<char>(c)));
                if (FT_Load_Char(Get().face, c, FT_LOAD_RENDER))
                {
                    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                    continue;
                }
                // generate texture
                Texture* texture = ResourceManager::CreateTexture(name,TexType::Tex2D, Get().face->glyph->bitmap.width, Get().face->glyph->bitmap.rows, false, GL_RED);
                texture->UpdateTexture(Get().face->glyph->bitmap.buffer, Get().face->glyph->bitmap.width, Get().face->glyph->bitmap.rows, GL_RED);

                float rows = static_cast<float>(Get().face->glyph->bitmap.rows);
                float width = static_cast<float>(Get().face->glyph->bitmap.width);

                std::vector<unsigned int> indices = {
                    0, 2, 1,
                    0, 3, 2
                };
                std::vector<Vertex> vertices = {
                    {
                        glm::vec3(0,-rows, 0),
                        glm::vec3(0,0,1),
                        glm::vec3(1,0,0),
                        glm::vec3(0,1,0),
                        glm::vec2(0,1),
                        glm::vec3(0)
                    },
                    {
                        glm::vec3(0,0, 0),
                        glm::vec3(0,0,1),
                        glm::vec3(1,0,0),
                        glm::vec3(0,1,0),
                        glm::vec2(0,0),
                        glm::vec3(0)
                    },
                    {
                        glm::vec3(width, 0, 0),
                        glm::vec3(0,0,1),
                        glm::vec3(1,0,0),
                        glm::vec3(0,1,0),
                        glm::vec2(1,0),
                        glm::vec3(0)
                    },
                    {
                        glm::vec3(width,-rows, 0),
                        glm::vec3(0,0,1),
                        glm::vec3(1,0,0),
                        glm::vec3(0,1,0),
                        glm::vec2(1,1),
                        glm::vec3(0)
                    },
                };

                Mesh* mesh = ResourceManager::LoadMesh(name, vertices, indices);
                Character character = {
                    texture,
                    glm::ivec2(Get().face->glyph->bitmap.width, Get().face->glyph->bitmap.rows),
                    glm::ivec2(Get().face->glyph->bitmap_left, Get().face->glyph->bitmap_top),
                    mesh,
                    Get().face->glyph->advance.x >> 6
                };
                Get().characters.insert(std::pair<char, Character>(c, character));
            }
            FT_Done_Face(Get().face);
            FT_Done_FreeType(Get().ft);
        }

        static void CreateText(SceneObject* obj, std::string& text, glm::vec3 color);
        static void CreateText(SceneObject *obj, std::string &text, Texture* tex, float uvScale = 1);

    };
} // BiBuild

#endif //VIEWER_TEXTGENERATOR_H
