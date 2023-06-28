varying vec2 v_uv;
uniform sampler2D u_texture;

void main()
{
	vec4 color = texture2D(u_texture, v_uv);
	float distance = length(v_uv - vec2(0.5));
	float radius = 0.9;
	float softness = 0.4;
	float vignette = smoothstep(radius, radius-softness, distance);
	color.rgb -= (1.0 - vignette);

	gl_FragColor = color;
}
