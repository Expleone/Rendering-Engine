#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aColor;

out vec3 FragPos;
out vec3 worldPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 VertexColor;

uniform mat4 model;
uniform mat3 normalMatrix;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

void main() {
    vec4 pos = model * vec4(aPos, 1.0f);
    worldPos = pos.xyz;

    // FIX 1: Keep FragPos in World Space! Do not multiply by 'view' here.
    FragPos = pos.xyz;

    Normal = normalize(normalMatrix * aNormal); // This is correctly in World Space

    TexCoords = aTexCoords;
    VertexColor = aColor;

    // Apply the view matrix here instead
    gl_Position = projection * view * pos;
}