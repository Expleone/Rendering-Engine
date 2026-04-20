#version 330 core

in vec3 TexCoords;
out vec4 fragmentColor;

uniform samplerCube cubeMapTex;

void main() {
    fragmentColor = texture(cubeMapTex, TexCoords);
}