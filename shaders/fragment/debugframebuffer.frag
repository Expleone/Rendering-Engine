#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform usampler2D fboTexture;

void main()
{
    uvec4 intColor = texture(fboTexture, TexCoords);

    FragColor = vec4(vec3(intColor.rgb) / 4294967295.0, 1.0);
}