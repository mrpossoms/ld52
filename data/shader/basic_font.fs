varying vec4 v_screen_pos;
varying vec2 v_uv;

uniform sampler2D u_texture;
uniform vec4 u_font_color;
uniform vec2 u_uv_top_left;
uniform vec2 u_uv_bottom_right;

varying vec4 color;

void main (void)
{
    vec2 uv = v_uv;

    float a = texture(u_texture, uv).a;
    color = u_font_color;
    // color.rg = uv;
    color *= a;
}
