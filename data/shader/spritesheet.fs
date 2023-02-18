varying vec2 v_uv;

uniform sampler2D u_sprite_sheet;
uniform vec2 u_sprite_sheet_size;
uniform vec2 u_sprite_sheet_frame_pos;
uniform vec2 u_sprite_sheet_frame_size;


void main (void)
{
	vec2 off = u_sprite_sheet_frame_pos / u_sprite_sheet_size;
	vec2 dims = u_sprite_sheet_frame_size / u_sprite_sheet_size;
	vec2 uv = off + v_uv * dims;

	gl_FragColor = texture(u_sprite_sheet, uv);
}
