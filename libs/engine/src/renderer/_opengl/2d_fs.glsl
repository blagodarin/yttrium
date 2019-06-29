#version 330

in vec4 io_color;
in vec2 io_texcoord;

uniform sampler2D surface_texture;

out vec4 o_color;

void main()
{
	o_color = io_color * texture(surface_texture, io_texcoord);
}
