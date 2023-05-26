uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;

varying vec4 v_color;

void main()
{
	vec3 total = u_Kd;

	gl_FragColor = vec4(total, 1.0);
}
