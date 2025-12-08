#version 330
precision mediump float;

in float v2f_time;
in vec3 v2f_point;
out vec4 f_color;

uniform bool greyscale;

//	Simplex 3D Noise
//	by Ian McEwan, Ashima Arts
//
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(vec3 v){
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod(i, 289.0 );
  vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
}

// fractal brownian motion
float fbm (in vec3 p) {
    // Initial values
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    //
    // Loop of octaves
    for (int i = 0; i < 6; i++) {
        value += amplitude * snoise(p);
        p *= 2.;
        amplitude *= .5;
    }
    return value;
}

float fbmO4 (in vec3 p) {
    // Initial values
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    //
    // Loop of octaves
    for (int i = 0; i < 4; i++) {
        value += amplitude * snoise(p);
        p *= 2.;
        amplitude *= .5;
    }
    return value;
}

// Ridged multifractal
// See "Texturing & Modeling, A Procedural Approach", Chapter 12
float ridge(float h, float offset) {
    h = abs(h);     // create creases
    h = offset - h; // invert so creases are at top
    h = h * h;      // sharpen creases
    return h;
}

float ridgedMF(vec3 p) {
    float lacunarity = 2.0;
    float gain = 0.5;
    float offset = 0.9;

    float sum = 0.0;
    float freq = 1.0, amp = 0.5;
    float prev = 1.0;
    for(int i=0; i < 6; i++) {
        float n = ridge(snoise(p*freq), offset);
        sum += n*amp;
        sum += n*amp*prev;  // scale by previous octave
        prev = n;
        freq *= lacunarity;
        amp *= gain;
    }
    return sum;
}


vec3 pattern1( in vec3 p , float ti)
{
    p*=5.0;

    // some noise
    float f = (0.5 + 0.5*fbm(p + 0.2*ti));
    float g = (0.5 + 0.5*fbm(p*0.5 + vec3(1.0,236.37,-23.34) - vec3(0.1f + f, 0.04*ti,f)));

    // base color
    vec3 color = vec3(0.9,0.34,0.03);

    // brighter regions
    color += smoothstep(0.1,0.35 + 0.05*sin(ti),f*g)*vec3(0.5,0.34,0.03);

    // some very bright regions
    color += smoothstep(0.35 +  0.05*sin(ti),0.55,f*g)*vec3(0.2,0.2,0.0);

    return clamp(color,0.0,1.0);
}

vec3 pattern2(in vec3 p, float ti)
{
    float scale = 2.0*(0.7 + 0.5*ridge(snoise(0.3*p + 0.1*ti),0.9));
    vec3 color = vec3(ridgedMF(p + ti))*vec3(1.0,0.5,0.0);
    color += (1.0 - color.x)*vec3(0.9,0.1,0.0);
    return clamp(color,0.0,1.0);
}

vec3 pattern3(in vec3 p, in float ti)
{
    vec3 orange = vec3(1.0,0.5,0.0);
    vec3 yellow = vec3(1.0,0.8,0.0);
    vec3 bright = vec3(1.0,0.9,0.7);
    vec3 red = vec3(0.8,0.2,0.0);

    // rough noise --> red spots
    float f = 0.5 + 0.5*fbm(p + 0.1*ti);
    float f2 = 0.5 + 0.5*fbmO4(p + vec3(8,-12,23) + 0.1*ti);
    float smoothf = smoothstep(0.4 + 0.05*sin(ti), 0.6,max(f,f2));
    vec3 color = mix(yellow,red,smoothf);

    // finer noise
    float g = 0.5 + 0.5*fbm(5.0*p + 0.5*ti);
    color = mix(color,orange,g);

    // bright center spots
    float smoothf_edge = smoothstep(0.25, 0.4 + 0.05*sin(ti),max(f,f2));
    color = mix(bright,color,smoothf_edge);

    return clamp(color,0.0,1.0);
}


void main()
{
    float ti = 0.04*v2f_time;

    vec3 p = v2f_point;

    vec3 color = pattern1(p,ti);
//    vec3 color = pattern2(p,ti);
//    vec3 color = pattern3(p,ti);


    // convert RGB color to YUV color and use only the luminance
    if (greyscale) color = vec3(0.299*color.r+0.587*color.g+0.114*color.b);

    f_color = vec4(color,1.0);
}
