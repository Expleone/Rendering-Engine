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
out vec2 textBlockUV;

uniform mat4 model;
uniform mat3 normalMatrix;

uniform vec2 symbolPos;
uniform vec2 totalTextSize;
uniform float textOriginY;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

void main() {
    vec3 offsetPos = aPos + vec3(symbolPos, 0.0);
    float textAspectRatio = totalTextSize.x/totalTextSize.y;
    textBlockUV.x = offsetPos.x / totalTextSize.x;
    textBlockUV.y = ((textOriginY - offsetPos.y) / totalTextSize.y) / textAspectRatio;

    // Apply the model matrix to the offset position
    vec4 pos = model * vec4(offsetPos, 1.0f);

    worldPos = pos.xyz;
    FragPos = pos.xyz;

    Normal = normalize(normalMatrix * aNormal);
    TexCoords = aTexCoords;
    VertexColor = aColor;

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 B = normalize(normalMatrix * aBitangent);

    TBN = mat3(T, B, N);

    gl_Position = projection * view * pos;
}