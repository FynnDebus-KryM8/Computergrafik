#version 330
precision mediump float;

in vec3 v2f_normal;
in vec2 v2f_texcoord;
in vec3 v2f_light;
in vec3 v2f_view;

out vec4 f_color;

uniform sampler2D day_texture;
uniform sampler2D night_texture;
uniform sampler2D cloud_texture;
uniform sampler2D gloss_texture;
uniform sampler2D normal_texture;
uniform bool greyscale;

const float shininess = 20.0;
const vec3  sunlight = vec3(1.0, 0.941, 0.898);

const float PI = 3.14159265358979323846;

vec3 gen_kuwahara(sampler2D src);
float sectorKernel(vec2 v, int k, int N, float radius);
vec3 kuwahara();
vec4 avg_dev_sector(vec2 starting_position, int kernel_size);
//bool inSector(float x, float y, int k);
float gaussian(vec2 v, float sigma);
mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv);
vec3 perturb_normal(vec3 N, vec3 V, vec2 texcoord);

void main()
{
    vec3 tex_color = texture(day_texture, v2f_texcoord).rgb;
    float alpha = texture(day_texture, v2f_texcoord).a;
    vec3 night = texture(night_texture, v2f_texcoord).rgb;
    float cloud = texture(cloud_texture, v2f_texcoord).r;
    float gloss = texture(gloss_texture, v2f_texcoord).r;

    vec3 normal = perturb_normal(normalize(v2f_normal), normalize(v2f_view), v2f_texcoord);

    float spec_factor = gloss * (1-cloud);
    float n_l = dot(normal, normalize(v2f_light));
    vec3 r = normalize(2.0f * normal * n_l - normalize(v2f_light));
    float r_v = dot(r, normalize(v2f_view));


    float night_factor = smoothstep(-1.0f, 1.0f, n_l);
    vec3 day_cloud = mix(tex_color, vec3(1.0f), cloud);
    vec3 night_cloud = mix(night, vec3(0.0f), cloud);
    vec3 color = mix(night_cloud, day_cloud, night_factor); // base color

    vec3 ambient = 0.2f * sunlight; // ambient
    vec3 diffuse = max(sunlight * n_l, vec3(0.0f, 0.0f, 0.0f));
    vec3 specular = max(spec_factor * sunlight * 0.2f * pow(r_v, shininess), vec3(0.0f, 0.0f, 0.0f));// specular

    color *= mix(vec3(1.0f), ambient + diffuse + specular, night_factor); // lighting

    // convert RGB color to YUV color and use only the luminance
    if (greyscale) color = vec3(0.299*color.r+0.587*color.g+0.114*color.b);

    // add required alpha value
    f_color = vec4(color, alpha);
    //    f_color = vec4(tex_color, alpha);
    //    f_color = vec4(kuwahara(), 1.0f);
    //    f_color = vec4(gen_kuwahara(day_texture), 1.0f);

    /** \todo Implement a fancy earth shader.
    * - Copy your working code from the fragment shader of your Phong shader use it as starting point
    * - instead of using a single texture, use the four texures `day_texure`, `night_texure`,
    *   `cloud_texure` and `gloss_texture` and mix them for enhanced effects:
    *       * The `gloss_texture` defines how glossy (specular) a point on earth is
    *       * The `cloud_texture` defines how cloudy a point on earth is, and more clouds should produce less reflections at this point
    *       * `day_texture` and `night_texture` define the day and night color of the earth. Make sure that there is a soft transition between both.
    *       * Note that there is no phong shading at the night side.
    * Hints:
    *   - cloud and gloss textures are just greyscales. So you'll just need one color component.
    *   - The texture(texture, 2d_position) returns a 4-vector (rgba). You can use either `texture(...).r` to get just the red component
    *     or `texture(...).rgb` to get a vec3 color value
    *   - use mix(vec3 a,vec3 b, float s) = a*(1-s) + b*s for linear interpolation of two colors
    *   - Lookup the documentation of the function called `smoothstep(...)` it might be helpful.
    *   - There is not the one right way to get the desired results. Feel free to use some magic numbers or creative solutions.
     */
}

// http://www.thetenthplanet.de/archives/1180
mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv) {
    vec3 dp1 = dFdx(p);
    vec3 dp2 = dFdy(p);
    vec2 duv1 = dFdx(uv);
    vec2 duv2 = dFdy(uv);

    vec3 dp2perp = cross(dp2, N);
    vec3 dp1perp = cross(N, dp1);
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
    return mat3(T * invmax, B * invmax, N);
}

vec3 perturb_normal(vec3 N, vec3 V, vec2 texcoord) {
    vec3 map = texture(normal_texture, texcoord).xyz;

    #ifdef WITH_NORMALMAP_UNSIGNED
    map = map * 255./127. - 128./127.;
    #endif
    #ifdef WITH_NORMALMAP_2CHANNEL
    map.z = sqrt(1. - dot(map.xy, map.xy));
    #endif
    #ifdef WITH_NORMALMAP_GREEN_UP
    map.y = -map.y;
    #endif
    mat3 TBN = cotangent_frame(N, -V, texcoord);
    return normalize(TBN * map);
}

// basic version of kuwahara filter
vec3 kuwahara() {
    ivec2 texSize = textureSize(day_texture, 0);// width, height of texture
    vec2 ts = 1.0 / vec2(texSize);// size of one texel in UV space

    int kernel_size = 7;
    int k_offset = int(kernel_size / 2);

    vec4 tl_area = avg_dev_sector(vec2(v2f_texcoord.x-k_offset*ts.x, v2f_texcoord.y-k_offset*ts.y), kernel_size);
    vec4 tr_area = avg_dev_sector(vec2(v2f_texcoord.x+k_offset*ts.x, v2f_texcoord.y-k_offset*ts.y), kernel_size);
    vec4 bl_area = avg_dev_sector(vec2(v2f_texcoord.x-k_offset*ts.x, v2f_texcoord.y+k_offset*ts.y), kernel_size);
    vec4 br_area = avg_dev_sector(vec2(v2f_texcoord.x+k_offset*ts.x, v2f_texcoord.y+k_offset*ts.y), kernel_size);

    vec4 smallest = tl_area;
    if (tr_area.a < smallest.a) smallest = tr_area;
    if (bl_area.a < smallest.a) smallest = bl_area;
    if (br_area.a < smallest.a) smallest = br_area;

    return smallest.rgb;

    //    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    //    result += tl_area.rgb * (1.0f / (1.0f + pow(255.0f * tl_area.a, 0.5f * 8.0f)));
    //    result += tr_area.rgb * (1.0f / (1.0f + pow(255.0f * tr_area.a, 0.5f * 8.0f)));
    //    result += bl_area.rgb * (1.0f / (1.0f + pow(255.0f * bl_area.a, 0.5f * 8.0f)));
    //    result += br_area.rgb * (1.0f / (1.0f + pow(255.0f * br_area.a, 0.5f * 8.0f)));
    //
    //    result /=
    //    (1.0f / (1.0f + pow(255.0f * tl_area.a, 0.5f * 8.0f))) +
    //    (1.0f / (1.0f + pow(255.0f * tr_area.a, 0.5f * 8.0f))) +
    //    (1.0f / (1.0f + pow(255.0f * bl_area.a, 0.5f * 8.0f))) +
    //    (1.0f / (1.0f + pow(255.0f * br_area.a, 0.5f * 8.0f)));
    //
    //    return result;
}

// kernelsize x kernelsize calculation of avg color and std. deviation
// starting from left-upper pixel
// returns vec4(avg-color, std-dev)
vec4 avg_dev_sector(vec2 starting_position, int kernel_size) {
    ivec2 texSize = textureSize(day_texture, 0);// width, height of texture
    vec2 ts = 1.0 / vec2(texSize);

    vec3 avg_color = vec3(0.0f, 0.0f, 0.0f);
    float mean = 0.0f;
    for (int x = 0; x < kernel_size; ++x) {
        for (int y = 0; y < kernel_size; ++y) {
            vec3 color = texture(day_texture, starting_position + vec2(ts.x * x, ts.y * y)).rgb;
            avg_color += color;
            mean += vec3(0.299*color.r+0.587*color.g+0.114*color.b).r;// luminance(?)
        }
    }
    avg_color /= kernel_size*kernel_size;
    mean /= kernel_size*kernel_size;

    float variance = 0.0f;
    for (int x = 0; x < kernel_size; ++x) {
        for (int y = 0; y < kernel_size; ++y) {
            vec3 color = texture(day_texture, starting_position + vec2(ts.x * x, ts.y * y)).rgb;
            float luminance = vec3(0.299*color.r+0.587*color.g+0.114*color.b).r;// luminance(?)

            variance += pow(luminance - mean, 2.0f);
        }
    }
    variance /= kernel_size*kernel_size;
    float std_dev = sqrt(variance);

    return vec4(avg_color, std_dev);
}

float gaussian(vec2 v, float radius) {
    float sigma = radius * 0.5f;
    return exp(-dot(v, v) / (2.0f * sigma * sigma));
}

// v = offset vector in pixel space
// k = sector index
// N = number of sectors
// radius = neighborhood radius
float sectorKernel(vec2 v, int k, int N, float radius) {
    // Normalize offset to [-1,1] range
    vec2 vn = v / radius;

    // Compute angle of this offset
    float angle = atan(vn.y, vn.x); // [-PI, PI]

    // Sector angle boundaries
    float sectorAngle = 2.0 * PI / float(N);
    float centerAngle = sectorAngle * float(k);

    // Angular distance from sector center
    float dtheta = angle - centerAngle;
    dtheta = mod(dtheta + PI, 2.0*PI) - PI; // wrap to [-PI, PI]

    // Radial Gaussian (distance from center)
    float sigma_r = radius * 0.5;
    float radial = exp(-dot(v,v) / (2.0 * sigma_r * sigma_r));

    // Angular Gaussian (distance from sector center)
    float sigma_theta = sectorAngle * 0.5;
    float angular = exp(-(dtheta*dtheta) / (2.0 * sigma_theta * sigma_theta));

    // Combined weight
    return radial * angular;
}

vec3 gen_kuwahara(sampler2D src) {
    // future parameters:

    const int N = 8;
    const int radius = 14;
    const float sharpness = 50.0f;
    float sigma = radius * 0.5f;

    // actual code:
    vec2 src_size = textureSize(src, 0);

    vec4 m[8];
    vec3 s[8];
    for (int k = 0; k < N; ++k) {
        m[k] = vec4(0.0f);
        s[k] = vec3(0.0f);
    }

    float piN = 2.0f * PI / float(N);
    mat2 X = mat2(cos(piN), sin(piN), -sin(piN), cos(piN));

    for (int j = -radius; j <= radius; ++j) {
        for (int i = -radius; i <= radius; ++i) {
            //            vec2 v = 0.5f * vec2(i, j) / float(radius);
            //            if (dot(v, v) <= 0.25f) {
            //                vec3 c = texture(src, v2f_texcoord + vec2(i, j) / src_size).rgb;
            //                for (int k = 0; k < N; ++k) {
            //                    float w = gaussian(v, float(radius));
            //                    m[k] += vec4(c * w, w);
            //                    s[k] += c * c * w;
            //                    v *= X;
            //                }
            //            }

            vec2 offset = vec2(i,j);
            if (dot(offset,offset) <= radius*radius) {
                vec3 c = texture(src, v2f_texcoord + (vec2(i, j) / src_size)).rgb;
                for (int k = 0; k < N; ++k) {
                    float w = sectorKernel(vec2(i,j), k, N, float(radius));
                    m[k] += vec4(c * w, w);
                    s[k] += c * c * w;
                    offset *= X;
                }
            }
        }
    }

    vec4 o = vec4(0.0f);
    for (int k = 0; k < N; ++k) {
        m[k].rgb /= m[k].w;
        s[k] = abs(s[k] / m[k].w - m[k].rgb * m[k].rgb);
        float sigma2 = s[k].r + s[k].g + s[k].b;
        float w = 1.0f / (1.0f + pow(255.0f * sigma2, 0.5f * sharpness));
        o += vec4(m[k].rgb * w, w);
    }

    return o.rgb / o.w;
}

//// fixes the stupid color system
//vec4 textureToRGBA(vec4 pixel) {
//    return vec4(pixel.g, pixel.b, pixel.a, pixel.r);
//}

//// Calculates:
//// \Sigma^7_{i=0}k_i * w_i / \Sigma^7_{i=0}w_i
//// with: w_i = 1/1+\sigma_i (std. variance of sector)
//// and: k_i = avg. color of sector
//vec3 gen_kuwahara() {
//    ivec2 texSize = textureSize(day_texture, 0);   // width, height of texture
//    vec2 ts = 1.0 / vec2(texSize);        // size of one texel in UV space
//
//    int kernel_size = 7;
//    float sigma = float(kernel_size) / 3.0;
//    ivec2 offset = ivec2(-int(kernel_size / 2), -int(kernel_size / 2)); // offset-vector from center of kernel
//
//    vec3 sum_kw = vec3(0.0f, 0.0f, 0.0f);
//    float sum_w = 0.0f;
//    for (int sector_index = 0; sector_index < 8; ++sector_index) {
//
//        float mean = 0.0f;
//        float weight_sum = 0.0f;
//        vec3 avg_color = vec3(0.0f, 0.0f, 0.0f);
//
//        // calculate mean
//        for (int pixel_index = 0; pixel_index < kernel_size*kernel_size; ++pixel_index) {
//            int x = pixel_index % kernel_size;
//            int y = int(pixel_index / kernel_size);
//
//            float dx = float(x) - float(kernel_size) / 2.0;
//            float dy = float(y) - float(kernel_size)/2.0;
//            if (inSector(dx, dy, sector_index)) {
//                vec3 color = texture(day_texture, v2f_texcoord + (vec2(offset)*ts) + vec2(x * ts.x, y * ts.y)).rgb;
//                float g = exp(-(dx*dx + dy*dy) / (2.0 * sigma*sigma));
//                avg_color += g * color;
//                float luminance = vec3(0.299*color.r+0.587*color.g+0.114*color.b).r; // luminance(?)
//
//                mean += g * luminance;
//                weight_sum += g;
//            }
//        }
//
//        if (weight_sum > 0.0f) {
//            mean /= weight_sum;
//            avg_color /= weight_sum;
//        }
//
//        float variance = 0.0f;
//        //variance calculation
//        for (int pixel_index = 0; pixel_index < kernel_size*kernel_size; ++pixel_index) {
//            int x = pixel_index % kernel_size;
//            int y = int(pixel_index / kernel_size);
//
//            float dx = float(x) - float(kernel_size) / 2.0;
//            float dy = float(y) - float(kernel_size)/2.0;
//            if (inSector(dx, dy, sector_index)) {
//                vec4 color = texture(day_texture, v2f_texcoord + (vec2(offset)*ts) + vec2(x * ts.x, y * ts.y));
//                float luminance = vec3(0.299*color.r+0.587*color.g+0.114*color.b).r; // luminance(?)
//
//                float g = exp(-(dx*dx + dy*dy) / (2.0 * sigma*sigma));
//                variance += g * pow(luminance - mean, 2.0f);
//            }
//        }
//        if (weight_sum > 0.0) {
//            variance /= weight_sum;
//        }
//        float w_i = 1.0f / (1.0f + sqrt(variance));
//
//        sum_kw += avg_color * w_i;
//        sum_w += w_i;
//    }
//    vec3 K = sum_kw / sum_w;
//
//    return K;
//}
//
//bool inSector(float x, float y, int k) {
//    if (x < 1e-3 && y < 1e-3 && x > -1e-3 && y > -1e-3) return true;
//
//    float angle = atan(-y, x);
//    if(angle < 0.0)
//    angle += 2.0 * 3.14159265;
//
//    float lower = ((2.0 * float(k) - 1.0) * 3.14159265) / 8.0;
//    float upper = ((2.0 * float(k) + 1.0) * 3.14159265) / 8.0;
//    return (angle > lower && angle <= upper);
//}
