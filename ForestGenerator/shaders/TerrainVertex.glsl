#version 330 core

/// @brief the vertex passed in
layout(location =0)in vec3 inVert;
layout(location =1)in vec3 inNormal;
layout(location =2)in vec2 inUV;

uniform float maxHeight;

uniform mat4 MVP;
uniform mat3 normalMatrix;
uniform mat4 MV;
uniform vec3 lightPos;
uniform float Kd = 0.9;
uniform float Ld = 1;

out vec3 vertColour;
out float lightIntensity;
out vec2 UV;

void main()
{
    ///@ref: Jon Macey

    // Convert normal and position to eye coords
    vec3 tnorm = normalize( normalMatrix * inNormal);
    vec4 eyeCoords = MV * vec4(inVert,1.0);
    vec3 s = normalize(vec3(lightPos - eyeCoords.xyz));
    // The diffuse shading equation
    lightIntensity = max( dot( s, tnorm ), 0.0 )* Ld * Kd;
    // Convert position to clip coordinates and pass along
    gl_Position = MVP*vec4(inVert,1.0);


  //gl_Position = MVP*vec4(inVert,1.0);

  float height = inVert[1];
  /*vertColour = mix(mix(vec3(0.2, 0.2, 0.05), vec3(0.3, 0.4, 0.1),
                       smoothstep(-1,0, height/maxHeight)),
                   vec3(0.4f,0.8f,0.2f),
                   smoothstep(0,1, height/maxHeight));*/

  UV = inUV;
  //vertColour = inNormal;
}
