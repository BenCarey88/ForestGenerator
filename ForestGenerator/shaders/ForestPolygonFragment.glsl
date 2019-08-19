#version 330 core

in vec3 vertColour;
in vec3 normal;
in vec3 worldPos;

uniform mat3 normalMatrix;
uniform mat4 MV;
uniform mat4 MVP;
uniform vec3 lightPos;
uniform float Kd = 0.9;
uniform float Ld = 1;

/// @brief our output fragment colour
layout (location =0)out vec4 fragColour;

void main ()
{
    //basic diffuse shading model @ref: Jon Macey
    vec3 tnorm = normalize( normalMatrix * normal);
    vec4 eyeCoords = MV * vec4(worldPos,1);
    vec3 s = normalize(vec3(lightPos - eyeCoords.xyz));
    float lightIntensity = max(dot(s,tnorm),0.0) * Ld * Kd;
    lightIntensity = (lightIntensity+1)*0.5;

    fragColour.rgb = lightIntensity * vertColour;
    fragColour.a = 1;
}
