#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec4 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;
    int texNum;
    sampler2D textures[16];
};

uniform samplerCube cubeMapTex;
uniform samplerCube nightCubeMapTex;
uniform vec3 sunPos;

uniform Material material;

bool isSun(){
    vec3 da = normalize(TexCoords);
    vec3 db = normalize(sunPos);
    return dot(da, db) > cos(0.087);
}

void main() {
    float sunSize = 0.1;
    vec3 viewDir = normalize(TexCoords);
    vec4 skyColor = texture(cubeMapTex, TexCoords);
    if(sunPos.y < 0.3) {
        skyColor = mix(skyColor, texture(nightCubeMapTex, TexCoords), 1.0-sunPos.y/0.3);
    }
    if(sunPos.y <= 0.0) {
        skyColor = texture(nightCubeMapTex, TexCoords);
    }

    float alignment = dot(viewDir, sunPos);

    if (alignment > 1.0 - sunSize) {

        vec3 worldUp = vec3(0.0, 1.0, 0.0);
        if (abs(sunPos.y) > 0.999) {
            worldUp = vec3(1.0, 0.0, 0.0);
        }

        vec3 right = normalize(cross(worldUp, sunPos));
        vec3 up = cross(sunPos, right);

        float projX = dot(viewDir, right);
        float projY = dot(viewDir, up);

        float u = (projX / sunSize) * 0.5 + 0.5;
        float v = (projY / sunSize) * 0.5 + 0.5;
        vec2 sunUV = vec2(u, v);

        vec4 sunTexColor = texture(material.textures[0], sunUV);

        FragColor = skyColor + sunTexColor;
    } else {
        FragColor = skyColor;
    }
}