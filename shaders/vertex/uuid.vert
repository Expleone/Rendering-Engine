#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

void main() {
    vec4 pos = model * vec4(aPos, 1.0f);

    gl_Position = projection * view * pos;
}