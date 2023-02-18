varying vec3 a_position;
varying vec2 a_uv;
varying vec3 a_normal;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;
varying vec2 v_uv;

void main (void)
{
	v_uv = a_uv;
	gl_Position = u_proj * u_view * u_model * vec4(a_position * 0.5, 1.0);
}
