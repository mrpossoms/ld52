in vec3 a_position;
in vec2 a_uv;
in vec3 a_normal;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;
out vec2 v_uv;

void main (void)
{
	v_uv = a_uv;
	gl_Position = u_proj * u_view * u_model * vec4(a_position * 0.5, 1.0);
}
