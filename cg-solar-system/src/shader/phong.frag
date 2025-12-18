#version 330
precision mediump float;

// output from phong.vert --> input from phong.frag
in vec3 v2f_normal;
in vec2 v2f_texcoord;
in vec3 v2f_light;
in vec3 v2f_view;

out vec4 f_color;

uniform sampler2D tex;
uniform bool greyscale;

const float shininess = 8.0;
const vec3  sunlight = vec3(1.0, 0.941, 0.898);

void main()
{
    /**
    *  Implement the Phong shading model (like in the first exercise)
    *  - Use the passed `in` variables to compute the resulting color and store it in `f_color`.
    *  - The texture's color value should be used as material parameter for ambient, diffuse and specular lighting.
    *  - You can copy the function to extract the texture's color from the `color.frag` fragmentshader.
    *  - Scale ambient and specular light component by 0.2 to get a more realistic result
    *  - You do not have to implement shadows.
    *
    *  Hint: Here, functions like reflect, dot, max, min, normalize can be used in the same way as in the raytracer.
     */
    vec3 tex_color = texture(tex, v2f_texcoord).rgb;
    float alpha = texture(tex, v2f_texcoord).a;

    vec3 color = tex_color;

    float n_l = dot(normalize(v2f_normal), normalize(v2f_light));
    vec3 r = normalize(2.0f * normalize(v2f_normal) * n_l - normalize(v2f_light));
    float r_v = dot(r, normalize(v2f_view));

    vec3 ambient = 0.2f * sunlight; // ambient
    vec3 diffuse = max(sunlight * n_l, vec3(0.0f, 0.0f, 0.0f));
    vec3 specular = max(sunlight * 0.2f * pow(r_v, shininess), vec3(0.0f, 0.0f, 0.0f));// specular

    color *= ambient + diffuse + specular;

//    if (!(n_l < 0.0f)) {
//        color += sunlight * tex_color * n_l; // diffuse
//        if (!(r_v < 0.0f)) {
//            color += sunlight * (0.2f * tex_color) * pow(r_v, shininess); // specular
////            color += sunlight * pow(r_v, shininess);
//        }
//    }

    // convert RGB color to YUV color and use only the luminance
    if (greyscale) color = vec3(0.299*color.r+0.587*color.g+0.114*color.b);

    // add required alpha value
    f_color = vec4(color, alpha);
}
