varying vec3 a_position;
varying vec2 a_uv;
varying vec3 a_normal;

uniform mat4 u_view;

varying vec2 v_uv;
varying vec3 v_cam_pos;

void main (void)
{
	v_uv = (a_uv - 0.5) * vec2(1.0, -1.0) + 0.5;
	v_cam_pos = u_view[3].xyz;
	gl_Position = vec4(a_position, 1.0);
}
