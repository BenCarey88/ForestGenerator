#version 330 core

in vec2 UV;

uniform sampler2D textureMap;
//uniform sampler2D normalMap;

/// @brief our output fragment colour
layout (location =0)out vec4 fragColour;

void main ()
{
    //add leaf texture (with alpha map for transparency)
    fragColour = texture(textureMap, UV);
}
