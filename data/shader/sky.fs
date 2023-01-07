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



	// vec3 sky_color = white * w_white + blue * w_blue + black * w_black; 
	vec3 sky_color = mix(white, blue, w);


	//sky_color *= clamp(1.0 / (-v_cam_pos.y * 0.01), 0.0, 1.0);


	color = vec4(sky_color, 1.0);
}
