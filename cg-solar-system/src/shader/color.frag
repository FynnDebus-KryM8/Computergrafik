#version 330
precision mediump float;

in  vec2 v2f_texcoord;
out vec4 f_color;

uniform sampler2D tex;
uniform bool greyscale;


void main()
{   
    // fetch color from texture
    vec3 color = texture(tex, v2f_texcoord.st).rgb;
    float alpha = texture(tex, v2f_texcoord.st).a;

    // convert RGB color to YUV color and use only the luminance
    if (greyscale) color = vec3(0.299*color.r+0.587*color.g+0.114*color.b);

    // add the required alpha value
    f_color = vec4(color.rgb, alpha);
}
