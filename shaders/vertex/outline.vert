#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

void main() {
    vec3 extrudedPos = aPos + (aNormal * 0.003);
    gl_Position = projection * view * model * vec4(extrudedPos, 1.0);
}