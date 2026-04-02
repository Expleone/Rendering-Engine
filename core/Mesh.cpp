//
// Created by expleoene on 3/30/26.
//

#include "Mesh.h"

#include "ShaderProgram.h"
#include "../helper_functions/helper.h"
namespace BiBuild{
    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& name)
        : id(Helper::genUUID()), name(name), vertices(vertices), indices(indices), vbo(vertices.data(), vertices.size() * sizeof(Vertex)), vao(), ebo(indices.data(), indices.size()) {
        vao.Bind();
        vbo.Bind();
        ebo.Bind();

        // Position attribute
        vao.LinkAttribute(vbo, static_cast<GLuint>(AttributeLocation::Position), 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
        // Normal attribute
        vao.LinkAttribute(vbo, static_cast<GLuint>(AttributeLocation::Normal), 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        // Texture coordinate attribute
        vao.LinkAttribute(vbo, static_cast<GLuint>(AttributeLocation::TexCoords), 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        // Color attribute
        vao.LinkAttribute(vbo, static_cast<GLuint>(AttributeLocation::Color), 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));

        vao.Unbind();
    }
}
