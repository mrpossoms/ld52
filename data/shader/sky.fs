varying vec2 v_uv;
varying vec3 v_cam_pos;


uniform mat4 u_view;

void main (void)
{
	vec3 white = vec3(0.529, 0.816, 0.922);
	vec3 blue = vec3(0.0, 0.749, 1.0);
	vec3 black = vec3(0.0, 0.0, 0.1);

	float w = v_uv.y - (u_view[3].y / 10.0);	

	w = clamp(w, 0.0, 1.0);

	vec3 sky_color = mix(white, blue, w);

	// blackness of space
	{
		float w = v_uv.y - ((u_view[3].y+10) / 20.0);
		sky_color = mix(sky_color, black, w);
	}

	gl_FragColor = vec4(sky_color, 1.0);
}
