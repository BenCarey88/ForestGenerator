#version 330 core

in vec3 colour;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec2 UV;
in vec3 worldPos;

/// @brief our output fragment colour
layout (location =0)out vec4 fragColour;

uniform sampler2D textureMap;
uniform sampler2D normalMap;

uniform mat3 normalMatrix;
uniform mat4 MV;
uniform mat4 MVP;
uniform vec3 lightPos;
uniform float Kd = 0.9;
uniform float Ld = 1;

void main ()
{
    mat3 TBN = mat3(tangent, bitangent, normal);

    vec3 newNormal = vec3(texture(normalMap, UV));
    newNormal = normalize(newNormal * 2.0 - 1);
    newNormal = normalize(TBN * newNormal);

    vec3 tnorm = normalize( normalMatrix * newNormal);
    vec4 eyeCoords = MV * vec4(worldPos,1);
    vec3 s = normalize(vec3(lightPos - eyeCoords.xyz));
    float lightIntensity = max(dot(s,tnorm),0.0) * Ld * Kd;

    //fragColour.rgb = lightIntensity * colour;

    fragColour = lightIntensity * texture(textureMap, UV);
}
