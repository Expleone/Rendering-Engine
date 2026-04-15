//
// Created by expleoene on 3/30/26.
//

#include "Mesh.h"

#include "ShaderProgram.h"
#include "../helper_functions/helper.h"
namespace BiBuild{
    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& name)
        : id(Helper::genUUID()), name(name), vertices(vertices), indices(indices), vbo(vertices.data(), vertices.size() * sizeof(Vertex)), vao(), ebo(indices.data(), indices.size()) {

    float minx = INFINITY, miny = INFINITY, minz = INFINITY, maxx = -INFINITY, maxy = -INFINITY, maxz = -INFINITY;

        for (auto& vertex : vertices) {
            minx = minx < vertex.position.x ? minx : vertex.position.x;
            miny = miny < vertex.position.y ? miny : vertex.position.y;
            minz = minz < vertex.position.z ? minz : vertex.position.z;

            maxx = maxx > vertex.position.x ? maxx : vertex.position.x;
            maxy = maxy > vertex.position.y ? maxy : vertex.position.y;
            maxz = maxz > vertex.position.z ? maxz : vertex.position.z;
        }

        bb.min = glm::vec3(minx, miny, minz);
        bb.max = glm::vec3(maxx, maxy, maxz);

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
        vao.LinkAttribute(vbo, static_cast<GLuint>(AttributeLocation::Tangent), 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        vao.LinkAttribute(vbo, static_cast<GLuint>(AttributeLocation::Bitangent), 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
        vao.Unbind();
    }
}
