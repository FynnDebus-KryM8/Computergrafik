#version 330

layout (location = 0) in vec4 v_position; // input vertex position
layout (location = 1) in vec3 v_normal;   // input vertex normal
layout (location = 2) in vec2 v_texcoord; // input texture coordinates

out vec2 v2f_texcoord;  // output: texture coordinates
out vec3 v2f_normal;    // output: normal at current point
out vec3 v2f_light;     // output: normalized light vector from current vertex to light source
out vec3 v2f_view;      // output: normalized view vector from current vertex to the eye

uniform mat4 modelview_projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;
uniform vec4 light_position; //in eye space coordinates already

void main()
{
    /** \todo Implement the phong vertex shader.
     *DONE? - Setup all `out` variables defined above. They will be the input of the phong fragment shader.
     *DONE - To do so you can use all other vectors and matrices above
     *DONE - Don't forget to compute the vertex position in screen space and store it in the global variable gl_Position
     *DONE? - In `Solar_Viewer::draw_scene(...)` you have to set all uniforms from both the vertex AND the pixel shader
     *
     * Hints: - Compute the vertex position, normal and light vector in eye space.
     *        - Store the final vertex position in `gl_Position`
     *        - Make sure that your vector sizes match
     *        - Via .xyz you can access the first three components of a vec4
     */
    v2f_texcoord = v_texcoord;
    vec3 eye_position = (modelview_matrix * v_position).xyz;
    v2f_normal = normalize(normal_matrix * v_normal);
    v2f_light = normalize(light_position.xyz - eye_position);
    v2f_view = normalize(-eye_position);
    gl_Position = modelview_projection_matrix * v_position;
}
