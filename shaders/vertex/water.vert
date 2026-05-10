#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aColor;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in vec3 aBitangent;

out vec3 FragPos;
out vec3 worldPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 VertexColor;
out mat3 TBN;

uniform mat4 model;
uniform mat3 normalMatrix;
uniform float time;

uniform float moveSpeed;
uniform float moveStrength;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

float calculateIndependentDisplacement(vec3 worldPos, float time, float speed, float strength) {
    float randomVal = hash(worldPos.xz);

    float phase = randomVal * 6.283185;

    float individualSpeed = speed * (0.8 + randomVal * 0.4);

    float movement = sin(time * individualSpeed + phase);

    return movement * strength;
}

void main() {
    vec4 pos = model * vec4(aPos, 1.0f);
    worldPos = pos.xyz;

    Normal = normalize(normalMatrix * aNormal);
    TexCoords = aTexCoords;
    VertexColor = aColor;

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 B = normalize(normalMatrix * aBitangent);

    TBN = mat3(T, B, N);

    float yOffset = calculateIndependentDisplacement(pos.xyz, time, moveSpeed, moveStrength);
    pos.y += yOffset;

    FragPos = pos.xyz;
    gl_Position = projection * view * pos;
}