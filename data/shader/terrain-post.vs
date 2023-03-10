in vec3 a_position;
in vec2 a_uv;
in vec3 a_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

out vec2 v_uv;
out vec4 v_world;

void main (void)
{
	v_uv = (a_uv - 0.5) * vec2(1.0, -1.0) + 0.5;
	v_world = u_model * vec4(a_position, 1.0);
	gl_Position = u_proj * u_view * v_world;
}
