#version 150

in vec3 v;
in vec3 n;

void main()
{
	vec3 nn = normalize(n);
	vec3 l = normalize(vec3(16.0, 32.0, 64.0) - v);
	vec3 e = normalize(-v);
	vec3 r = normalize(-reflect(l, nn));
	vec3 ambient = vec3(0.1, 0.0, 0.0);
	vec3 diffuse = vec3(0.3, 0.1, 0.1) * clamp(dot(nn, l), 0.0, 1.0);
	vec3 specular = vec3(0.3, 0.0, 0.0) * clamp(dot(r, e), 0.0, 1.0);
	gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}
