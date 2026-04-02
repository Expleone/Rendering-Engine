#version 330 core

in vec3 VertexColor;
out vec4 fragmentColor;

void main() {
    fragmentColor = vec4(VertexColor, 1.0f);
}