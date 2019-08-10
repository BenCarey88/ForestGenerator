#version 330 core

/// @brief the vertex passed in
layout(location =0)in vec3 inVert;
layout(location =1)in mat4 inTransform;
layout(location =5)in vec3 inRightVector;
layout(location =6)in float inThicknessValues;

uniform mat4 MVP;

out vec3 vertCol;
out vec3 worldSpacePos;
out vec3 rightVector;
out float thickness;

void main()
{
  //Note that this is identical to the treeVertex shader except that
  //we multiply the positions by transform
  gl_Position = MVP * inTransform * vec4(inVert,1.0);
  vertCol = vec3(inVert[0]/10,inVert[1]/10,0);

  worldSpacePos = vec3(inTransform * vec4(inVert,1));
  rightVector = inRightVector;
  thickness = inThicknessValues;
}
