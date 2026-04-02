#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aColor;
in vec3 aNextPos; // Next position for animation
in float t; // Interpolation factor

out vec3 FragPos;
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
    vec3 pos = mix(aPos, aNextPos, t);
    vec4 worldPos = model * vec4(pos, 1.0f);
    FragPos = worldPos.xyz;

    Normal = normalize(normalMatrix * aNormal);

    TexCoords = aTexCoords;
    VertexColor = aColor;

    gl_Position = projection * view * worldPos;
}
void main() {
    vec3 pos = mix(aPosition, aNextPositions, t);
    gl_Position = PVM * vec4(pos, 1.0f);
}