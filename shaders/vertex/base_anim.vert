#version 330 core
in vec3 aPosition;
in vec3 aNextPositions;
uniform mat4 PVM;
uniform float t;

void main() {
    vec3 pos = mix(aPosition, aNextPositions, t);
    gl_Position = PVM * vec4(pos, 1.0f);
}