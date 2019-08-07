#version 330 core

/// @brief the vertex passed in
layout(location =0)in vec3 inVert;
layout(location =1)in vec3 inNormal;
layout(location =2)in vec2 inUV;
layout(location =3)in vec3 inTangent;
layout(location =4)in vec3 inBitangent;

uniform float maxHeight;

uniform mat4 MVP;
uniform mat4 M;

out vec3 vertColour;
out float lightIntensity;
out vec2 UV;
out mat3 TBN;
out vec3 worldPos;

void main()
{
    ///@ref: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    vec3 T = normalize(vec3(M * vec4(inTangent, 0)));
    vec3 B = normalize(vec3(M * vec4(inBitangent, 0)));
    vec3 N = normalize(vec3(M * vec4(inNormal, 0)));
    mat3 TBN = mat3(T, B, N);

    gl_Position = MVP*vec4(inVert,1.0);


  //gl_Position = MVP*vec4(inVert,1.0);

  //float height = inVert[1];
  /*vertColour = mix(mix(vec3(0.2, 0.2, 0.05), vec3(0.3, 0.4, 0.1),
                       smoothstep(-1,0, height/maxHeight)),
                   vec3(0.4f,0.8f,0.2f),
                   smoothstep(0,1, height/maxHeight));*/

  UV = inUV;
  worldPos = inVert;

  //vertColour = inNormal;
}
