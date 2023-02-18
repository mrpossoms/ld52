varying vec4 v_screen_pos;
varying vec2 v_uv;

uniform sampler2D u_texture;
uniform vec4 u_font_color;
uniform vec2 u_uv_top_left;
uniform vec2 u_uv_bottom_right;

void main (void)
{
    vec2 uv = v_uv;

    float a = texture(u_texture, uv).a;
    vec4 color = u_font_color;
    // color.rg = uv;
    color *= a;
    gl_FragColor = color;
}
