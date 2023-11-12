#version 450 core

layout(location = 0) in vec2 a_Offset;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in float a_TexIndex;

uniform mat4 u_MVP;

out vec2 v_TexCoord;
out float v_TexIndex;

void main()
{
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;

	vec4 pos = vec4(a_Offset, 0.0, 1.0);
	gl_Position = u_MVP * pos;
}