#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location=0) in vec4 i_color;
layout(location=1) in vec2 i_texcoord;

layout(binding=1) uniform sampler2D u_texture;

layout(location=0) out vec4 o_color;

void main()
{
	o_color = i_color * texture(u_texture, i_texcoord);
}
