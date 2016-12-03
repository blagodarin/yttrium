#version 330

layout(location = 0) in vec4 in_position;

uniform mat4 u_modelview;
uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * u_modelview * in_position;
}
