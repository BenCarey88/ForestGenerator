#version 330 core

/// @brief the vertex passed in
layout(location =0)in vec3 inVert;
layout(location =1)in vec3 inRightVector;

uniform mat4 MVP;
out vec3 vertCol;
out vec3 worldSpacePos;
out vec3 rightVector;

void main()
{
  gl_Position = MVP*vec4(inVert,1.0);
  vertCol = vec3(inVert[0]/10,inVert[1]/10,0);

  worldSpacePos = inVert;
  rightVector = inRightVector;
}
