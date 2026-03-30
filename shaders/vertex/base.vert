#version 330 core
in vec3 aPosition;
uniform mat4 PVM;

void main() {
    vec3 pos = aPosition;
    gl_Position = PVM * vec4(pos, 1.0f);
}