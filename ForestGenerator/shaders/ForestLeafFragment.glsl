#version 330 core

in vec2 UV;


uniform sampler2D textureMap;

/// @brief our output fragment colour
layout (location =0)out vec4 fragColour;

void main ()
{
    fragColour = texture(textureMap, UV);
    fragColour.a = step(0.0000000000001, fragColour.r+fragColour.g+fragColour.b);
}
