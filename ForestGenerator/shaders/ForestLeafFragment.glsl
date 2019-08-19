#version 330 core

in vec2 UV;


uniform sampler2D textureMap;

/// @brief our output fragment colour
layout (location =0)out vec4 fragColour;

void main ()
{
    //texture with leaf map with alpha channel for transparency
    fragColour = texture(textureMap, UV);
}
