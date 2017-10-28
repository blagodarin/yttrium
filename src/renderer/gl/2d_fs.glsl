#version 330

in vec4 io_color;
in vec2 io_texcoord;

uniform sampler2D surface_texture;

void main()
{
	gl_FragColor = io_color * texture2D(surface_texture, io_texcoord);
}
