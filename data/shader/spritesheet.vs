in vec3 a_position;
in vec2 a_uv;
in vec3 a_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

uniform vec2 u_sprite_sheet_size;
uniform vec2 u_sprite_sheet_frame_pos;
uniform vec2 u_sprite_sheet_frame_size;

out vec2 v_uv;

void main (void)
{
	v_uv = a_uv;

	vec3 aspect = vec3(1.0, u_sprite_sheet_frame_size.y / u_sprite_sheet_frame_size.x, 1.0);

	gl_Position = u_proj * u_view * u_model * vec4(a_position * aspect, 1.0);
}
