#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in vec3 origPos[3];

out vec3 vertColour;
out vec3 normal;
out vec3 worldPos;

void main ()
{
    //use geometry shader to get normals for basic diffuse shading model
    vec3 globalNormal = cross(origPos[1]-origPos[0],origPos[2]-origPos[0]);

    for(int i=0; i<3; i++)
    {
        gl_Position = gl_in[i].gl_Position;
        normal = globalNormal;
        vertColour = vec3(0.3,0.5,0.2);
        worldPos = origPos[i];
        EmitVertex();
    }
    EndPrimitive();
}
