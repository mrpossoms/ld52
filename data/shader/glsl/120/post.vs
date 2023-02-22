attribute vec3 a_position;
attribute vec2 a_uv;
attribute vec3 a_normal;

varying vec2 v_uv;

void main (void)
{
	v_uv = (a_uv - 0.5) * vec2(1.0, -1.0) + 0.5;
	gl_Position = vec4(a_position, 1.0);
}
