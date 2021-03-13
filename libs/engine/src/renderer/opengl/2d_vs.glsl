#version 330

layout(location=0) in vec2 in_position;
layout(location=1) in vec2 in_texcoord;
layout(location=2) in vec4 in_color;

uniform mat4 mvp;

out vec4 io_color;
out vec2 io_texcoord;

void main()
{
	gl_Position = mvp * vec4(in_position, 0, 1);
	io_color = in_color;
	io_texcoord = in_texcoord;
}
