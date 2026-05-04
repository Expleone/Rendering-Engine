#version 330 core



in vec3 VertexColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;
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
    vec4 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;
    int texNum;
    sampler2D textures[16];
};

layout (std140) uniform Lights {
    Light lights[100];
    int numLights;
};

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

layout (std140) uniform Fog {
    vec4 fogColor;
    float fogDistanceClose;
    float fogDistanceFar;
    int useSkybox;
};
uniform samplerCube fogTex;

uniform Material material;


vec3 CalculateLighting(vec3 normal) {
    vec3 result = vec3(0.0);

    vec3 amb_mat = material.ambient;
    vec3 diff_mat = material.diffuse.xyz;
    if(material.texNum >= 1) {
        diff_mat = diff_mat*texture(material.textures[0], TexCoords).xyz;
        amb_mat = amb_mat*texture(material.textures[0], TexCoords).xyz;
    }
    vec3 viewPos = inverse(view)[3].xyz;
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


        vec3 cameraDir = normalize(viewPos - FragPos);

        vec3 ambient = lightAmb*amb_mat;
        vec3 diffuse = vec3(0.0);
        vec3 specular = vec3(0.0);

        if(type == 0){
            float diffStrength = max(dot(-lightDir, normal), 0.0);
            diffuse = diffStrength * lightDiff * diff_mat;
            if(diffStrength > 0.0){
                vec3 halfwayDir = normalize(lightDir + cameraDir);
                float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
                specular = spec * lightSpec * material.specular;
                //                specular = pow(max(dot(reflect(lightDir, normal), cameraDir),0), material.shininess)*lightSpec*material.specular;

            }
            //

            result += ambient + diffuse + specular;
            continue;
        }else{
            float attenuationFactor = 1.0 / (constant + linear*dist + quadratic*dist*dist);
            vec3 lDir = normalize(lightPos - FragPos);
            float diffStrength = max(dot(lDir, normal), 0.0);
            diffuse = diffStrength * lightDiff * diff_mat;
            if(diffStrength > 0.0){
                vec3 halfwayDir = normalize(lDir + cameraDir);
                float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
                specular = spec * lightSpec * material.specular;
                //                specular = pow(max(dot(reflect(-lDir, normal), cameraDir),0), material.shininess)*lightSpec*material.specular;
            }
            //
            float spotLightEffect = 1.0;
            if(type == 2){
                if (dot(-lDir, lightDir) < cos(cutoff)){
                    spotLightEffect = 0.0;
                }
            }
            result += ambient + spotLightEffect*attenuationFactor*(diffuse + specular);
        }
    }
    return result;
}

void main() {
    float alpha = material.diffuse.w;
    if(material.texNum >= 1) {
        alpha *= texture(material.textures[0], TexCoords).w;
    }
    if (alpha < 0.01) discard;

    vec3 fog = fogColor.xyz;
    vec3 viewPos = inverse(view)[3].xyz;
    float dis = distance(viewPos, FragPos);
    float fogCoeficient = 0.0;
    if(dis >= fogDistanceClose) fogCoeficient = clamp((dis - fogDistanceClose)/(fogDistanceFar-fogDistanceClose), 0.0, 1.0);
    if(fogCoeficient==1) discard;
    if(useSkybox == 1) fog = texture(fogTex, FragPos - viewPos).rgb;
    vec4 color = vec4(mix(texture(material.textures[0], TexCoords).rgb, fog, fogCoeficient), alpha);

    fragmentColor = color;
}