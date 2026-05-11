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
uniform vec3 windDir;
uniform float windStrength;
uniform float windSpeed;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};


vec3 calculateWindDisplacement(vec3 worldPos, float time, float speed, float strength, vec3 direction) {
    float spatialOffset = (worldPos.x + worldPos.z) * 0.5;

    float sway = sin(time * speed + spatialOffset);

    float flutter = sin(time * (speed * 3.14) + spatialOffset * 2.7) * 0.25;

    float totalMovement = (sway + flutter) * strength;

    return normalize(direction) * totalMovement;
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
    vec3 disp = calculateWindDisplacement(pos.xyz, time, windSpeed, windStrength, windDir);
    pos = pos + vec4(disp ,0);
    FragPos = pos.xyz;
    // Apply the view matrix here instead
    gl_Position = projection * view * pos;

}