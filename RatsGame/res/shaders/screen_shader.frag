#version 450 core

#define PI 3.14159265359

out vec4 o_FragColor;
  
in vec2 v_TexCoords;
in vec2 v_Pos;

uniform sampler2D u_ScreenTexture;
uniform float u_ScreenScale;
uniform float u_PlayerAngle;

mat2 Rot(float a)
{
    float s = sin(a);
    float c = cos(a);
    return mat2(c, -s, s, c);
}

void main()
{ 
    vec4 col = texture(u_ScreenTexture, v_TexCoords);

    vec2 pos = vec2(v_Pos.x * u_ScreenScale, v_Pos.y);
    pos *= Rot(-PI / 2.);
    pos.x *= .6;
    pos.x += .45;
    pos.y *= .5;

    float x = atan(pos.y, pos.x);

    float d = length(pos) + x * x * .2 - .0;
    col.rgb *= 1. - smoothstep(0.85, 1.0, d);

    o_FragColor = col;
}