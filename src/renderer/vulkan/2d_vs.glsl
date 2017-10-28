#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(std140,binding=0) uniform buffer
{
	mat4 mvp;
} u_buffer;

layout(location=0) in vec4 i_position;
layout(location=1) in vec4 i_color;

layout(location=0) out vec4 o_color;

void main()
{
	o_color = i_color;
	gl_Position = u_buffer.mvp * i_position;
}
