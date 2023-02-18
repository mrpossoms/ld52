varying vec2 v_uv;
uniform sampler2D u_tex;

void main (void)
{
	gl_FragColor = texture(u_tex, v_uv);
}
