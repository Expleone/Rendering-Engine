#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat3 normalMatrix;
layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);

    vec3 worldNormal = normalize(normalMatrix * aNormal);

    worldPos.xyz += worldNormal * 0.002;

    gl_Position = projection * view * worldPos;
}