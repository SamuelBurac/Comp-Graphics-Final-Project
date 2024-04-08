#version 330 core

uniform vec2 iResolution;
uniform float iTime;
uniform int iFrame;
in vec2 fragCoord; // screen space coordinate
out vec4 fragColor;

#define NUM_EXPLOSIONS 5
#define NUM_PARTICLES 75
#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define Time (iTime * 10.)

// return random vec2 between 0 and 1
vec2 hash2d(float t)
{
    float x = fract(sin(t * 674.3) * 453.2);
    float y = fract(sin((t + x) * 714.3) * 263.2);

    return vec2(x, y);
}

vec2 hash2d_polar(float t)
{
    float a = fract(sin(t * 674.3) * 453.2) * TWO_PI;
    float d = fract(sin((t + a) * 714.3) * 263.2);
    return vec2(sin(a), cos(a)) * d;
}

float explosion(vec2 uv, float t)
{
    float sparks = 0.;
    for(float i = 0.; i < NUM_PARTICLES; i++)
    {
        vec2 dir = hash2d_polar(i + 1.) * .5;
        float d = length(uv - dir * t);
        float brightness = mix(.0005, .002, smoothstep(.05, 0., t)); // brightness changes over time
        brightness *= sin(t * 20 + i) * .5 + .5; // flicker
        brightness *= smoothstep(1., .75, t); // fade out
        sparks += brightness / d;
    }
    return sparks;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = (fragCoord - .5 * iResolution.xy) / iResolution.y;
    vec3 col = vec3(0.0);

    for(float i = 0.; i < NUM_EXPLOSIONS; i++)
    {
        float t = Time + i / NUM_EXPLOSIONS;
        float ft = floor(t);
        vec3 color = sin(40. * vec3(0.34, 0.54, 0.43) * ft) * 0.25 + 0.75;
        vec2 offs = hash2d(i + 1. + ft) - .5;
        offs *= vec2(1.77, 1.);
        col += vec3(explosion(uv - offs, fract(t))) * color;
    }
    col *= 2.;
    fragColor = vec4(col, 1.0);
}

void main()
{
    mainImage(fragColor, fragCoord);
}
