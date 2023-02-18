varying vec3 a_position;
varying vec2 a_uv;
varying vec3 a_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

uniform vec2 u_sprite_sheet_size;
uniform vec2 u_sprite_sheet_frame_pos;
uniform vec2 u_sprite_sheet_frame_size;

varying vec2 v_uv;

void main (void)
{
	v_uv = a_uv;

	float s_y = u_sprite_sheet_frame_size.y / u_sprite_sheet_frame_size.x;
	float scale = u_sprite_sheet_frame_size.x / 200.0;
	vec3 aspect = vec3(1.0, s_y, 1.0) * scale;

	gl_Position = u_proj * u_view * u_model * vec4(a_position * aspect, 1.0);
}
