#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_world_pos;
varying vec3 v_normal;
varying vec3 v_tangent;
varying vec3 v_up;
varying mat3 v_basis;

varying vec4 color;

void main (void)
{
    color = vec4(v_normal * 0.5 + 0.5, 1.0);
}
