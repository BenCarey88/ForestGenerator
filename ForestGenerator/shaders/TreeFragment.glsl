#version 330 core

in vec3 colour;

/// @brief our output fragment colour
layout (location =0)out vec4 fragColour;

void main ()
{
    fragColour.rgb = colour;
}
