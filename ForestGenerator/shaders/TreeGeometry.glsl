#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices=18) out;

in vec3 vertCol[2];
in vec3 worldSpacePos[2];
in vec3 rightVector[2];
in float thickness[2];

out vec3 colour;
out vec3 normal;
out vec3 tangent;
out vec3 bitangent;
out vec2 UV;
out vec3 worldPos;

uniform mat4 MVP;

void main ()
{
    //vec3 lineDir = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 lineDir = worldSpacePos[1] - worldSpacePos[0];
    lineDir = normalize(lineDir);
    vec3 N = rightVector[1];
    vec3 E = cross(lineDir,N);
    vec3 NE = normalize(N+E);
    vec3 SE = normalize(E-N);

    vec3[9] compass = vec3[9](N,NE,E,SE,-N,-NE,-E,-SE,N);

    for(int i=0; i<9; i++)
    {
        gl_Position = MVP * vec4(worldSpacePos[0] + 0.5*thickness[0]*compass[i], 1);
        colour = vertCol[0];
        normal = compass[i];
        tangent = compass[(i+2)%8];
        bitangent = cross(normal, tangent);
        UV = vec2(float(i)/9,0);
        worldPos = worldSpacePos[0];
        EmitVertex();

        gl_Position = MVP * vec4(worldSpacePos[1] + 0.5*thickness[1]*compass[i], 1);
        colour = vertCol[1];
        normal = compass[i];
        tangent = compass[(i+2)%8];
        bitangent = cross(normal, tangent);
        UV = vec2(float(i)/9,1);
        worldPos = worldSpacePos[0];
        EmitVertex();
    }

    EndPrimitive();
}
