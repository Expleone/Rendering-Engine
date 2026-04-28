#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

// Must use usampler2D to read GL_RGBA32UI or GL_RGBA_INTEGER formats
uniform usampler2D fboTexture;

void main()
{
    uvec4 intColor = texture(fboTexture, TexCoords);

    // Scale the 32-bit integer values down to a visible float range.
    // 4294967295.0 is the maximum value of a 32-bit unsigned integer.
    // You may need to divide by a smaller number if your UUID components are small,
    // otherwise the screen will appear mostly black.
    FragColor = vec4(vec3(intColor.rgb) / 4294967295.0, 1.0);
}