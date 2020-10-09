#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Normal;
layout (location = 2) in vec2 a_UVs;

out vec3 b_Normal;
out vec2 b_UVs;

uniform mat4 u_Mat;

void main()
{
	b_Normal = a_Normal.rgb;
	b_UVs = a_UVs;

	gl_Position = u_Mat * vec4(a_Position, 1.0f);
}