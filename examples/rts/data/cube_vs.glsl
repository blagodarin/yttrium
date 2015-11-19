#version 330

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec4 in_color;

uniform mat4 u_modelview;
uniform mat4 u_projection;

out vec4 color;

void main()
{
	gl_Position = u_projection * u_modelview * in_position;
	color = in_color;
}
