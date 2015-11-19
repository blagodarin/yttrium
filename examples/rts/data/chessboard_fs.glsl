#version 150

in vec2 texcoord;
in vec3 view_direction;
in vec3 n;

uniform sampler2D surface_texture;

void main()
{
	vec4 c = texture2D(surface_texture, texcoord.xy);
	if (view_direction.x < 0.0)
	{
		float gray = dot(c.xyz, vec3(1.0, 1.0, 1.0));
		gl_FragColor = vec4(gray, gray, gray, c.a * 0.25);
	}
	else
	{
		vec3 l = normalize(-view_direction);
		vec3 e = normalize(-view_direction);
		vec3 r = normalize(-reflect(l, n));
		vec3 diffuse = c.rgb * clamp(dot(n, l), 0.0, 1.0);
		vec3 specular = vec3(1.0, 1.0, 1.0) * clamp(dot(r, e), 0.0, 1.0);
		gl_FragColor = vec4(diffuse + specular, c.a);
	}
}
