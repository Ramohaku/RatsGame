#version 450 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;
in float v_TexIndex;

uniform sampler2D u_Textures[32];

void main()
{
	int index = int(v_TexIndex);
    vec4 col = texture(u_Textures[index], v_TexCoord);
    //vec4 col = vec4(vec3(v_TexIndex), 1.0f);

    o_Color = col;
}