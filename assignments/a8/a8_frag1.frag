#version 330 core

uniform vec2 iResolution;
uniform float iTime;
uniform int iFrame;
in vec2 fragCoord; // screen space coordinate
out vec4 fragColor;

#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define Gravity 0.8 // gravity
#define NUM_PARTICLES 30.
#define Time (iTime * 20.)

// return random vec2 between 0 and 1
vec2 hash2d(float t)
{
    float x = fract(sin(t * 674.3) * 453.2);
    float y = fract(sin((t + x) * 714.3) * 263.2);

    return vec2(x, y);
}
// return a random vec2 on a circle
vec2 hash2d_polar(float t)
{
    float a = fract(sin(t * 674.3) * 453.2) * TWO_PI;
    float d = fract(sin((t + a) * 714.3) * 263.2);
    return vec2(sin(a), cos(a)) * d;
}

vec3 simSingleParticle(vec2 uv)
{
    vec3 col = vec3(0.0);

    float duration = 2.;
    float t = mod(Time, duration);

    vec2 initPos = vec2(0.0, -0.5);
    vec2 vel = vec2(0.2, 1.2);
    vec2 g = vec2(0.0, -Gravity);
    vec2 pos = initPos + vel * t + 0.5 * g * t * t;

    float d = length(uv - pos);
    float brightnes = .002;
    col += brightnes / d;

    return col;
}

vec3 simSingleFirework(vec2 uv)
{
    vec3 col = vec3(0.0);

    float duration = 3.;
    float t = mod(Time, duration);

    vec2 initPos = vec2(-0.3, -0.5);
    vec2 vel = vec2(0.2, 1.2);
    vec2 g = vec2(0.0, -Gravity);
    vec2 pos = initPos + vel * t + 0.5 * g * t * t;
    // 1.5 unit to arise, 1.5 unit to explode
    if(t < 1.5)
    {
        float d = length(uv - pos);
        float brightnes = .002;
        col += brightnes / d;
    }
    else
    {
        float t2 = t - 1.5; // t - 1.5 is the explosion duration
        for(float i = 0.; i < NUM_PARTICLES; i++)
        {
            vec2 dir = hash2d_polar(i + 1.) * 0.8; // random direction with max magnitude 0.8
            vec2 pos2 = pos + dir * t2; // update position
            float d = length(uv - pos2);
            float brightness = mix(.001, .002, smoothstep(.05, 0., t2)); // brightness changes over time
            brightness *= sin(t2 * 20. + i) * .5 + .5; // flicker
            brightness *= smoothstep(1., .75, t2); // fade out
            col += brightness / d;
        }
    }

    return col;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    // uv's center is at the center of the screen, uv.y range is [-0.5, 0.5]
    vec2 uv = (fragCoord - .5 * iResolution.xy) / iResolution.y;

    vec3 col = simSingleFirework(uv);
    fragColor = vec4(col, 1.0);
}

void main()
{
    mainImage(fragColor, fragCoord);
}
