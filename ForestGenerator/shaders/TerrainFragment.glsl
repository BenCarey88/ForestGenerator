#version 330 core

in vec3 vertColour;
in float lightIntensity;
in vec2 UV;
in mat3 TBN;
in vec3 origPos;

in vec3 Tout;
in vec3 Bout;
in vec3 Nout;

in vec3 normal;

/// @brief our output fragment colour
layout (location =0)out vec4 fragColour;

uniform sampler2D textureMap;
uniform sampler2D normalMap;

uniform mat3 normalMatrix;
uniform mat3 newNormalMatrix;
uniform mat4 MV;
uniform mat4 MVP;
uniform vec3 lightPos;
uniform float Kd = 0.9;
uniform float Ld = 1;

void main ()
{
    //fragColour.rgb = vec3(lightIntensity,lightIntensity,lightIntensity);

    //fragColour.rgb = lightIntensity*vertColour;

    mat3 TBN1 = mat3(Tout, Bout, Nout);

//    vec3 normal = vec3(texture(normalMap, UV));
//    normal = normalize(normal * 2.0 - 1);
//    normal = normalize(TBN1 * normal);

    ///@ref: Jon Macey
    // Convert normal and position to eye coords
    vec3 tnorm = normalize( newNormalMatrix * normal);
    vec4 eyeCoords = MV * vec4(origPos,1.0);
    vec3 s = normalize(vec3(lightPos - eyeCoords.xyz));
    // The diffuse shading equation
    float lightIntensity = max( dot( s, tnorm ), 0.0 )* Ld * Kd;

//    fragColour = vec4(normal,1);
    //scale lightIntensity to range [0.3,1] to make things slightly brighter
    lightIntensity = (lightIntensity+0.3)*0.769;

    fragColour = lightIntensity * texture(textureMap, UV);

    //fragColour.rgb = lightIntensity * vec3(0.5,0.3,0.2);

    fragColour.a = 1;// step(0.0000001, fragColour.r+fragColour.g+fragColour.b);

    //fragColour = vec4(normal,1);
    //fragColour = vec4(vertColour,1);

    //fragColour = vec4(TBN1[0][0], TBN1[1][0], TBN1[2][0], 1);

    //fragColour = vec4(Tout,1);

    /*vec2 pos = mod(gl_FragCoord.xy, vec2(50.0)) - vec2(25.0);
    float dist_squared = dot(pos, pos);
    fragColour.rgba = mix(vec4(.90, .90, .90, 1.0), vec4(.20, .20, .40, 1.0),
                         smoothstep(380.25, 420.25, dist_squared));*/
}
