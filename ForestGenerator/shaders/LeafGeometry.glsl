#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices=4) out;

in vec4 dir[1];
in vec4 right[1];

uniform mat4 MVP;

out vec2 UV;

void main ()
{
    //use right and direction vectors to turn point to plane

    gl_Position = gl_in[0].gl_Position + 0.5*right[0];
    UV = vec2(1,1);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + 0.5*right[0] + dir[0];
    UV = vec2(1,0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position - 0.5*right[0];
    UV = vec2(0,1);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position - 0.5*right[0] + dir[0];
    UV = vec2(0,0);
    EmitVertex();

    EndPrimitive();
}
