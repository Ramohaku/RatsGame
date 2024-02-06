#version 450 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;
in float v_TexIndex;
in vec2 v_Position;

uniform sampler2D u_Textures[32];

#define MAX_LIGHTS_COUNT 30

uniform int u_LightsCount;
uniform vec4 u_LightsColors[MAX_LIGHTS_COUNT]; // a - active
uniform vec4 u_LightsData[MAX_LIGHTS_COUNT]; // x,y - position, z - vanish, w - distance

// only for editor, remove in final release
uniform int u_AllLight;

float Light(float d, float vanish, float distance)
{
    return clamp(distance / d - vanish, 0.0, 3.0); // clamp(.56 * (-2. * atan(d * vanish - distance) / 3.1415 + 1.), 0., 1.);
}

void main()
{
    int index = int(v_TexIndex);
    vec4 col = texture(u_Textures[index], v_TexCoord);

    if (u_AllLight == 0)
    {
        vec3 lightCol = vec3(0.);
        for (int i = 0; i < u_LightsCount; i++)
        {
            //if (!bool(u_LightsColors[i].a))
            if (u_LightsColors[i].a == 0.0)
                continue;

            float d = length(v_Position - u_LightsData[i].xy);
            float fn = Light(d, u_LightsData[i].z, u_LightsData[i].w);
            lightCol += u_LightsColors[i].rgb * fn;
        }

        col.rgb *= lightCol;
    }

    o_Color = col;
}