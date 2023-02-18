varying vec2 v_uv;
uniform sampler2D u_tex;
varying vec4 color;

void main (void)
{
	color = texture(u_tex, v_uv);
}
