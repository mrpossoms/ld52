in vec2 v_uv;

out vec4 color;

uniform sampler2D u_tex;

void main (void)
{
	color = texture(u_tex, v_uv);
}
