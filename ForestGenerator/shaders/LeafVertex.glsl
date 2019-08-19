#version 330 core

/// @brief the vertex passed in
layout(location =0)in vec3 inVert;
layout(location =1)in vec3 inDir;
layout(location =2)in vec3 inRight;

uniform mat4 MVP;
//out vec3 vertColour;
out vec4 dir;
out vec4 right;

void main()
{
  gl_Position = MVP*vec4(inVert,1.0);
  dir = MVP*vec4(inDir,0);
  right = MVP*vec4(inRight,0);
}
