#version 450 core

out vec4 o_FragColor;
  
in vec2 v_TexCoords;
in vec2 v_Pos;

uniform sampler2D u_ScreenTexture;
uniform float u_ScreenScale;

void main()
{ 
    vec4 col = texture(u_ScreenTexture, v_TexCoords);

    vec2 pos = vec2(v_Pos.x * u_ScreenScale, v_Pos.y);

    float d = length(pos);
    col.rgb *= 1. - smoothstep(0.85, 1.0, d);

    o_FragColor = col;
}