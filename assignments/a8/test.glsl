

#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define Gravity 0.7 // gravity
#define NUM_STAR 50.
#define NUM_PARTICLES 30.
#define NUM_FIREWORKS 3.
#define DURATION 3.

const vec2 g = vec2(0.0, -Gravity);

// return random float between 0 and 1
float hash1d(float t)
{
    t += 1.;
    return fract(sin(t * 674.3) * 453.2);
}

// return random vec2 between 0 and 1
vec2 hash2d(float t)
{
    t += 1.;
    float x = fract(sin(t * 674.3) * 453.2);
    float y = fract(sin((t + x) * 714.3) * 263.2);

    return vec2(x, y);
}
// return random vec3 between 0 and 1
vec3 hash3d(float t)
{
    t += 1.;
    float x = fract(sin(t * 674.3) * 453.2);
    float y = fract(sin((t + x) * 714.3) * 263.2);
    float z = fract(sin((t + y) * 134.3) * 534.2);

    return vec3(x, y, z);
}

// return a random vec2 on a circle
vec2 hash2d_polar(float t)
{
    t += 1.;
    float a = fract(sin(t * 674.3) * 453.2) * TWO_PI;
    float d = fract(sin((t + a) * 714.3) * 263.2);
    return vec2(sin(a), cos(a)) * d;
}

vec3 starBackground(vec2 uv)
{
    vec3 col = vec3(0.0);

    float t = iTime;
    for(float i = 0.; i < NUM_STAR; i++)
    {
        vec2 pos = hash2d(i) * 2. - 1.;
        float d = length(uv - pos);
        float brightness = mix(.0009, .001, smoothstep(.05, 0., t)); // brightness changes over time
        brightness *= sin(1.5 * t + i) * .5 + .5; // flicker

        col += brightness / d;
    }

    return col;
}

vec3 simSingleParticle(vec2 uv)
{
    vec3 col = vec3(0.0);

    float duration = 2.;
    float t = mod(iTime, duration);

    vec2 initPos = vec2(0.0, -0.5);
    vec2 vel = vec2(0.2, 1.);
    vec2 pos = initPos + vel * t + 0.5 * g * t * t;

    float d = length(uv - pos);
    float brightnes = .002;
    col += brightnes / d;

    return col;
}
// launchTime: the timestamp when the firework is launched
// t: the time since the firework is launched
// idx: the firework index
vec3 simSingleFirework(vec2 uv, vec2 initPos, vec2 vel,  float idx, float launchTime, float t)
{
    vec3 col = vec3(0.0);

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
        float t2 = t - 1.5;
        vec2 pos = initPos + vel * 1.5 + 0.5 * g * 1.5 * 1.5;
        for(float i = 0.; i < NUM_PARTICLES; i++)
        {
            vec2 dir = hash2d_polar(i) * .86; // random direction with max magnitude 0.8
            vec2 pos2 = pos + dir * t2; // update position
            float d = length(uv - pos2);
            float brightness = mix(.001, .002, smoothstep(.05, 0., t2)); // brightness changes over time
            brightness *= sin(t2 * 10. + i) * .5 + .5; // flicker
            brightness *= smoothstep(1., .75, t2); // fade out
            col += brightness / d;
        }
    }
    col *= 2.; // make it brighter
    // tint
    vec3 tint = sin(40. * hash3d(launchTime) * idx) * 0.25 + 0.75;

    return col * tint;
}

vec3 fireworks(vec2 uv)
{
    vec3 col = vec3(0.0);

    for (float i = 0.; i < NUM_FIREWORKS; i++)
    {
        float relTime = iTime - i;
        float t = mod(relTime, DURATION);
        float idx = floor(relTime / DURATION);
        vec2 iniPos = vec2((hash1d(idx) * 2. - 1.) * 0.7, -0.5);
        vec2 initVel = vec2(-iniPos.x * 0.66, hash1d(i + 1.) * 0.3 + .9);
        col += simSingleFirework(uv, iniPos, initVel, idx, i, t);
    }

    return col;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    // uv's center is at the center of the screen, uv.y range is [-0.5, 0.5]
    vec2 uv = (fragCoord - .5 * iResolution.xy) / iResolution.y;
    // uv.y = uv.y + 0.5; // make uv.y [0, 1]

    vec3 col = starBackground(uv);

    // float lauchTime = 0.;
    // float relTime = iTime - lauchTime;
    // float t = mod(relTime, DURATION);
    // float idx = floor(relTime / DURATION);
    // col += simSingleFirework(uv, vec2(-0.3, -0.5), vec2(0.2, .95), idx, lauchTime, t);


    col += fireworks(uv);
    fragColor = vec4(col, 1.0);
}