#version 330 core

/// @brief the vertex passed in
layout(location =0)in vec3 inVert;

uniform mat4 MVP;
out vec3 vertColour;

void main()
{
  gl_Position = MVP*vec4(inVert,1.0);

  float height = inVert[1];
  vec3 col;
  if (height > 0.8f)
  {
      col = vec3(1,1,1);
  }
  else if (height > 0.4f)
  {
    col  = vec3(0.5,0.5,0.5);
  }
  else if (height > -0.4f)
  {
    col  = vec3(0.4f,0.8f,0);
  }
  else if (height > -0.8f)
  {
    col  = vec3(0.25f,0.2f,0);
  }
  else
  {
    col  = vec3(0.2f,0.2f,0);
  }

  vertColour = col;
}
