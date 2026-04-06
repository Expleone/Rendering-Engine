#version 330 core



in vec3 VertexColor;
in vec3 FragPos;
in vec3 Normal;
out vec4 fragmentColor;


struct Light {
    // relative to camera position + w component determines light type (0 for directional, 1 for point, 2 for spotlight)
    vec4 position;
    vec4 ambient; // + w component can be used for directional/spot light direction x
    vec4 diffuse; // + w component can be used for directional/spot light direction y
    vec4 specular; // + w component can be used for directional/spot light direction z
    vec4 attenuation; // x = constant, y = linear, z = quadratic, w = cutoff (for spotlights)
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess; // Determines the size and focus of the specular highlight
};

layout (std140) uniform Lights {
    Light lights[100];
    int numLights;
};

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

uniform Material material;

vec3 CalculateLighting(vec3 normal, Material material) {
    vec3 result = vec3(0.0);

    for(int i = 0; i < numLights; i++){
        vec3 lightPos = lights[i].position.xyz;
        vec3 lightAmb = lights[i].ambient.xyz;
        vec3 lightDiff = lights[i].diffuse.xyz;
        vec3 lightSpec = lights[i].specular.xyz;

        float constant = lights[i].attenuation.x;
        float linear = lights[i].attenuation.y;
        float quadratic = lights[i].attenuation.z;

        int type = int(lights[i].position.w);
        vec3 lightDir = vec3(lights[i].ambient.w, lights[i].diffuse.w, lights[i].specular.w);
        float cutoff = lights[i].attenuation.w;

        float dist = distance(lightPos, FragPos);
        vec3 viewPos = inverse(view)[3].xyz;

        vec3 cameraDir = normalize(viewPos - FragPos);

        vec3 ambient = lightAmb*material.ambient;
        vec3 diffuse = vec3(0.0);
        vec3 specular = vec3(0.0);

        if(type == 0){
            diffuse = max(dot(-lightDir, normal), 0.0) * lightDiff * material.diffuse;
            specular = pow(max(dot(reflect(lightDir, Normal), cameraDir),0), material.shininess)*lightSpec*material.specular;
            result += (ambient + diffuse + specular);
            continue;
        }else{
            float attenuationFactor = 1.0 / (constant + linear*dist + quadratic*dist*dist);
            vec3 lDir = normalize(lightPos - FragPos);
            diffuse = max(dot(lDir, normal), 0.0) * lightDiff * material.diffuse;
            specular = pow(max(dot(reflect(-lDir, Normal), cameraDir),0), material.shininess)*lightSpec*material.specular;
            float spotLightEffect = 1.0;
            if(type == 2){
                if (dot(-lDir, lightDir) < cos(cutoff)){
                    spotLightEffect = 0.0;
                }
            }
            result += spotLightEffect*attenuationFactor*(ambient + diffuse + specular);
        }
    }
    return result;
}

void main() {
    vec3 color = material.emission + CalculateLighting(Normal, material);

    fragmentColor = vec4(color, 1.0f);
}