#version 330 core

/// @brief the vertex passed in
layout(location =0)in vec3 inVert;
layout(location =1)in vec3 inNormal;
layout(location =2)in vec3 inTangent;
layout(location =3)in vec3 inBitangent;
layout(location =4)in vec2 inUV;

uniform float maxHeight;

uniform mat4 MVP;
uniform mat4 M;

out vec3 Tout;
out vec3 Bout;
out vec3 Nout;

out vec3 normal;

out vec3 vertColour;
out float lightIntensity;
out vec2 UV;
out mat3 TBN;
out vec3 origPos;

void main()
{
    ///@ref: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    //TBN matrix for normal map
    vec3 T = normalize(vec3(M * vec4(inTangent,   0)));
    vec3 B = normalize(vec3(M * vec4(inBitangent, 0)));
    vec3 N = normalize(vec3(M * vec4(inNormal,    0)));
    mat3 TBN = mat3(T, B, N);

    Tout = T;
    Bout = B;
    Nout = N;

    gl_Position = MVP*vec4(inVert,1.0);

    vertColour = inBitangent*0.5+0.5;

    UV = inUV;
    origPos = inVert;
    normal = normalize(vec3(M * vec4(inNormal,    0)));

}
