#version 330

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec4 in_normal;

uniform mat4 u_modelview;
uniform mat4 u_projection;

out vec2 texcoord;
out vec3 view_direction;
out vec3 n;

void main()
{
	view_direction = vec3(u_modelview * in_position);
	n = normalize(transpose(inverse(mat3(u_modelview))) * vec3(in_normal));
	gl_Position = u_projection * u_modelview * in_position;
	texcoord = in_texcoord;
}
