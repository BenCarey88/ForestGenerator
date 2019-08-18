#version 330 core

/// @brief the vertex passed in
layout(location =0)in vec3 inVert;
layout(location =1)in mat4 inTransform;

uniform mat4 MVP;

out vec3 origPos;

void main()
{
  gl_Position = MVP * inTransform * vec4(inVert,1.0);
  origPos  = vec3(inTransform * vec4(inVert,1.0));
}
