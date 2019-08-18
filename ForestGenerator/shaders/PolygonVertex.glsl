#version 330 core

/// @brief the vertex passed in
layout(location =0)in vec3 inVert;

uniform mat4 MVP;

out vec3 origPos;

void main()
{
  gl_Position = MVP*vec4(inVert,1.0);
  origPos = inVert;
}
