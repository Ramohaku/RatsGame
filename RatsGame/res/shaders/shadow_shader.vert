#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_Offset;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_Rotation;
layout(location = 5) in float a_LightIndex;
layout(location = 6) in float a_LightActive;

uniform mat4 u_MVP;

out vec2 v_TexCoord;
out float v_TexIndex;
out float v_LightIndex;
out float v_LightActive;
out vec2 v_Position;

void main()
{
	float s = sin(a_Rotation);
	float c = cos(a_Rotation);
	mat2 rot = mat2(
		c, -s,
		s,  c
	);

	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_LightIndex = a_LightIndex;
	v_LightActive = a_LightActive;

	vec4 pos = vec4(a_Position + rot * a_Offset, 0.0, 1.0);
	v_Position = pos.xy;
	
	gl_Position = u_MVP * pos;
}