#version 330

in vec3 v;
in vec2 texcoord;
in vec3 n;

uniform sampler2D surface_texture;

out vec4 color;

void main()
{
	vec3 nn = normalize(n);
	vec4 c = texture2D(surface_texture, texcoord.xy);
	vec3 l = normalize(vec3(0, 0, 8) - v);
	vec3 e = normalize(-v);
	vec3 r = normalize(-reflect(l, nn));
	vec3 diffuse = c.rgb * clamp(dot(nn, l), 0.0, 1.0);
	vec3 specular = vec3(0.5, 0.5, 0.5) * clamp(dot(r, e), 0.0, 1.0);
	color = vec4(diffuse + specular, c.a);
}
