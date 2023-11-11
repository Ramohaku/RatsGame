#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in float a_Rotation;
//layout(location = 1) in vec4 a_Color;

out vec4 v_Color;

void main()
{
	float s = sin(a_Rotation);
	float c = cos(a_Rotation);
	
	vec2 position = a_Position * mat2(c, -s, s, c);
	v_Color.r = position.x;
	v_Color.g = 1. - position.y;

	gl_Position = vec4(position, 0.0, 1.0);
}