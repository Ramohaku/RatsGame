#version 450 core

out vec4 o_FragColor;
  
in vec2 v_TexCoords;

uniform sampler2D u_ScreenTexture;

void main()
{ 
    o_FragColor = texture(u_ScreenTexture, v_TexCoords);
}