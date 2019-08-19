#version 330 core

in vec3 vertColour;
in float lightIntensity;
in vec2 UV;
in mat3 TBN;
in vec3 origPos;

in vec3 Tout;
in vec3 Bout;
in vec3 Nout;

in vec3 normal;

/// @brief our output fragment colour
layout (location =0)out vec4 fragColour;

uniform sampler2D textureMap;
uniform sampler2D normalMap;

uniform mat3 normalMatrix;
uniform mat3 newNormalMatrix;
uniform mat4 MV;
uniform mat4 MVP;
uniform vec3 lightPos;
uniform float Kd = 0.9;
uniform float Ld = 1;

void main ()
{
    mat3 TBN1 = mat3(Tout, Bout, Nout);

//NOTE: normal map not currently loading, hence TBN currently unused

//    vec3 normal = vec3(texture(normalMap, UV));
//    normal = normalize(normal * 2.0 - 1);
//    normal = normalize(TBN1 * normal);

    ///basic diffuse modl @ref: Jon Macey
    // Convert normal and position to eye coords
    vec3 tnorm = normalize( newNormalMatrix * normal);
    vec4 eyeCoords = MV * vec4(origPos,1.0);
    vec3 s = normalize(vec3(lightPos - eyeCoords.xyz));
    // The diffuse shading equation
    float lightIntensity = max( dot( s, tnorm ), 0.0 )* Ld * Kd;

    //scale lightIntensity to range [0.3,1] to make things slightly brighter
    lightIntensity = (lightIntensity+0.3)*0.769;

    fragColour = lightIntensity * texture(textureMap, UV);

    //slightly alter hues of fragment to change terrain colour
    fragColour.g = (fragColour.g+0.1)/1.1;
    fragColour.b = (fragColour.b+0.1)/1.1;

    fragColour.a = 1;
}
