#version 330 core
in vec3 FragPos;
in vec3 VertexColor;
out vec4 fragmentColor;

void main() {
    vec3 color = normalize(FragPos) ; // Normalize position to [0, 1] range
    fragmentColor = vec4(color, 1.0f);
}