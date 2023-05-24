
varying vec3 v_uv;

uniform samplerCube u_texture;

void main()
{
	vec3 uv = v_uv;
	gl_FragColor = textureCube( u_texture, uv );
}
