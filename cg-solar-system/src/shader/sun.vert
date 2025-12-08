#version 330

layout (location = 0) in vec4 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texcoord;

out vec2 v2f_texcoord;
out float v2f_time;
out vec3 v2f_point;

uniform mat4 modelview_projection_matrix;
uniform float time;


void main()
{    
    // pass through texture coordinate
    v2f_texcoord = v_texcoord;
    
    // pass through time
    v2f_time = time;

    // pass through vertex position
    v2f_point = v_position.xyz;

    // transform vertex by modelview and projection matrix
    gl_Position = modelview_projection_matrix * v_position;

} 
