in vec2 v_uv;
in vec3 v_cam_pos;

out vec4 color;

void main (void)
{
	vec3 white = vec3(1.0, 1.0, 1.0);
	vec3 blue = vec3(0.0, 0.749, 1.0);
	vec3 black = vec3(0.0, 0.0, 0.1);

	float w = v_uv.y - (v_cam_pos.y / 10.0);	

	w = clamp(w, 0.0, 1.0);

	vec3 sky_color = mix(white, blue, w);

	// blackness of space
	{
		float w = v_uv.y - ((v_cam_pos.y+10) / 20.0);
		sky_color = mix(sky_color, black, w);
	}

	color = vec4(sky_color, 1.0);
}
