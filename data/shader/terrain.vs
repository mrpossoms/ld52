#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec3 a_tangent;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

varying vec3 v_normal;
varying vec3 v_tangent;
varying vec3 v_up;
varying vec4 v_world_pos;
varying mat3 v_basis;

void main (void)
{
   v_world_pos = u_model * vec4(a_position, 1.0);
   gl_Position = u_proj * u_view * v_world_pos;

    mat3 model_rot = mat3(normalize(u_model[0].xyz), normalize(u_model[1].xyz), normalize(u_model[2].xyz));
    v_normal = normalize(model_rot * a_normal);
    v_tangent = normalize(model_rot * a_tangent);
    v_up = normalize(a_position);

    v_basis = mat3(v_tangent, v_normal, cross(v_tangent, v_normal));
}
