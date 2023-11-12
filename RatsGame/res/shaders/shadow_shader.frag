#version 450 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;
in float v_TexIndex;
in float v_LightIndex;
in float v_LightActive;
in vec2 v_Position;

uniform sampler2D u_Textures[32];

#define MAX_LIGHTS_COUNT 30

uniform int u_LightsCount;
uniform vec4 u_LightsData[MAX_LIGHTS_COUNT]; // x,y - position, z - vanish, w - distance

float Light(float d, float vanish, float distance)
{
    return clamp(distance / d - vanish, 0.0f, 3.0f); //clamp(.56 * (-2. * atan(d * vanish - distance) / 3.1415 + 1.), 0., 1.);
}

void main()
{
	int texIndex = int(v_TexIndex);
    vec4 col = texture(u_Textures[texIndex], v_TexCoord);

	int lightIndex = int(v_LightIndex);
	col.rgb = vec3(0.);
    
    bool lightNotActive = !bool(v_LightActive);
	for (int i = 0; i < u_LightsCount; i++)
    {
        if (v_LightActive < 0.5)
        {
            col.a = 0.0;
            continue;
        }

        float d = length(v_Position - u_LightsData[i].xy);
        float fn = Light(d, u_LightsData[i].z, u_LightsData[i].w);
        if (i != lightIndex)
        {
            col.a -= fn * .25;
        }
        else
        {
            col.a *= fn * 1.8;
        }
    }

	o_Color = col;
}