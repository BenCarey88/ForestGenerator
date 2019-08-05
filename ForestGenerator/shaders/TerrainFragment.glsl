#version 330 core

in vec3 vertColour;
in float lightIntensity;

/// @brief our output fragment colour
layout (location =0)out vec4 fragColour;

void main ()
{
    //fragColour.rgb = vec3(lightIntensity,lightIntensity,lightIntensity);

    fragColour.rgb = lightIntensity*vertColour;


    /*vec2 pos = mod(gl_FragCoord.xy, vec2(50.0)) - vec2(25.0);
    float dist_squared = dot(pos, pos);
    fragColour.rgba = mix(vec4(.90, .90, .90, 1.0), vec4(.20, .20, .40, 1.0),
                         smoothstep(380.25, 420.25, dist_squared));*/
}
