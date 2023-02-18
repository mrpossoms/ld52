varying vec2 v_uv;
varying vec3 v_cam_pos;

varying vec4 color;

uniform mat4 u_view;
uniform mat4 u_proj;

uniform vec3 u_ship_pos;
uniform vec2 u_ship_down;

uniform float u_power;
uniform float u_beam_angle;
uniform float u_time;

void main (void)
{
	vec4 p = (u_proj * u_view * vec4(u_ship_pos, 1.0));
	vec3 screen_pos = p.xyz / p.w;

	vec2 delta = (v_uv.xy - 0.5) - screen_pos.xy;
	float dist = length(delta);
	float angle = acos(dot(delta, u_ship_down) / dist);
	if (angle <= u_beam_angle)
	{
		float a = 1.0 / (dist * 10.0);
		a *= 1.0 - angle / u_beam_angle;
		color = mix(vec4(0.5, 1.0, 0.5, 1.0), vec4(0.0, 1.0, 0.0, 1.0), angle / u_beam_angle);
		color.a = a * ((0.5 * cos(u_time * 40) + 0.5) * 0.25 + 0.75);

		color *= u_power;
	}
	else
	{
		discard;
	}
}
