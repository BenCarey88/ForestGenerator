#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices=10) out;

in vec3 vertCol[2];
in vec3 worldSpacePos[2];
in vec3 rightVector[2];

out vec3 colour;

uniform mat4 MVP;

void main ()
{
    //vec3 lineDir = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 lineDir = worldSpacePos[1] - worldSpacePos[0];
    lineDir = normalize(lineDir);
    vec3 right = rightVector[1];
    vec3 k = cross(lineDir,right);

    float a = 0.5;

    /*vec3 xDir = cross(lineDir,vec3(0,1,0));
    if(length(xDir)<0.0000001)
    {
        xDir = cross(lineDir,vec3(1,0,0));
    }
    vec3 zDir = cross(lineDir,xDir);
    xDir = normalize(xDir);
    zDir = normalize(zDir);
    mat3 rotMVP = mat3(MVP);
    xDir = rotMVP * xDir;
    zDir = rotMVP * zDir;

    vec4 x = vec4(xDir,1);
    vec4 z = vec4(zDir,1);*/

    gl_Position = MVP * vec4(worldSpacePos[0] + a*right, 1);
    colour = vertCol[0];
    EmitVertex();

    gl_Position = MVP * vec4(worldSpacePos[1] + a*right, 1);
    colour = vertCol[1];
    EmitVertex();

    gl_Position = MVP * vec4(worldSpacePos[0] + a*k, 1);
    colour = vertCol[0];
    EmitVertex();

    gl_Position = MVP * vec4(worldSpacePos[1] + a*k, 1);
    colour = vertCol[1];
    EmitVertex();

    gl_Position = MVP * vec4(worldSpacePos[0] - a*right, 1);
    colour = vertCol[0];
    EmitVertex();

    gl_Position = MVP * vec4(worldSpacePos[1] - a*right, 1);
    colour = vertCol[1];
    EmitVertex();

    gl_Position = MVP * vec4(worldSpacePos[0] - a*k, 1);
    colour = vertCol[0];
    EmitVertex();

    gl_Position = MVP * vec4(worldSpacePos[1] - a*k, 1);
    colour = vertCol[1];
    EmitVertex();

    gl_Position = MVP * vec4(worldSpacePos[0] + a*right, 1);
    colour = vertCol[0];
    EmitVertex();

    gl_Position = MVP * vec4(worldSpacePos[1] + a*right, 1);
    colour = vertCol[1];
    EmitVertex();

    EndPrimitive();

    /*gl_Position = gl_in[0].gl_Position + x;
    colour = vertCol[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + x;
    colour = vertCol[1];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + z;
    colour = vertCol[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + z;
    colour = vertCol[1];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position - x;
    colour = vertCol[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position - x;
    colour = vertCol[1];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position - z;
    colour = vertCol[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position - z;
    colour = vertCol[1];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + x;
    colour = vertCol[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + x;
    colour = vertCol[1];
    EmitVertex();

    EndPrimitive();*/

}
