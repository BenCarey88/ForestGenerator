#version 330 core

/// @brief the vertex passed in
layout(location =0)in vec3 inVert;

layout(location =1)in mat4 transform;

uniform mat4 MVP;
out vec3 vertColour;

void main()
{
  //gl_Position = MVP*vec4(inVert,1.0);
  gl_Position = MVP*transform*vec4(inVert,1.0);
  vertColour = vec3(inVert[0]/10,inVert[1]/10,0);
}
