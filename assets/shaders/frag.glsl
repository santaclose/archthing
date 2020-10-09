#version 450 core

layout (location = 0) out vec4 o_Color;

in vec3 b_Normal;
in vec2 b_UVs;

uniform vec3 u_LightDir;
uniform int u_NoLighting;
uniform int u_UseTexture;
uniform sampler2D u_Texture;

void main()
{
	if (u_NoLighting != 0)
	{
		o_Color = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else
	{
		float diff = max(dot(normalize(-b_Normal), normalize(u_LightDir)), 0.0);
		vec4 textureColor = vec4(1.0f);texture(u_Texture, b_UVs) * u_UseTexture + vec4(1 - u_UseTexture);
		o_Color = vec4(vec3(diff) * textureColor.rgb, 1.0);
	}
}