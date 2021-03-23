#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location=0) in vec2 i_position;
layout(location=1) in vec2 i_texcoord;
layout(location=2) in vec4 i_color;

layout(std140,binding=0) uniform buffer
{
	mat4 mvp;
} u_buffer;

layout(location=0) out vec4 o_color;
layout(location=1) out vec2 o_texcoord;

void main()
{
	gl_Position = u_buffer.mvp * vec4(i_position, 0, 1);
	o_color = i_color;
	o_texcoord = i_texcoord;
}
