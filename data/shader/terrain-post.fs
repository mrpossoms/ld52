#define M_PI 3.14159

varying vec2 v_uv;
varying vec4 v_world;

uniform mat4 u_view;
uniform mat4 u_proj;

uniform sampler2D u_under_ground;
uniform sampler2D u_above_ground;

void main (void)
{
	float d = -v_world.y + sin(v_world.x / 10) + sin(v_world.x / 3) * 0.3f + sin(v_world.x + M_PI / 2) * 0.125f - 3;

	if (d < 0) discard;

	vec2 uv = v_world.xy * 0.5;

	vec4 color = mix(texture(u_above_ground, uv), texture(u_under_ground, uv), clamp(d, 0.0, 1.0));

	vec4 blue = vec4(0.0, 0.749, 1.0, 1.0);

	gl_FragColor = mix(color, blue, clamp(-v_world.z / 7.0, 0.0, 1.0));
}
