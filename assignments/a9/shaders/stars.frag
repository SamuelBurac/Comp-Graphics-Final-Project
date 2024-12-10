#version 330 core

uniform vec2 iResolution;
uniform float iTime;
uniform int iFrame;

uniform sampler2D tex_buzz; 

in vec3 vtx_pos;
in vec2 vtx_uv;

out vec4 frag_color;

#define NUM_SNOW 100.0
#define GRAVITY 0.1

vec2 hash2d(float t)
{
    t += 1.0;
    float x = fract(sin(t * 674.3) * 453.2);
    float y = fract(sin((t + x) * 714.3) * 263.2);

    return vec2(x, y);
}

vec3 renderParticle(vec2 uv, vec2 pos, float brightness, vec3 color)
{
    float d = length(uv - pos);
    return brightness / d * color;
}

vec2 moveParticle(vec2 initPos, float t)
{
    vec2 currentPos = initPos;
    currentPos.y -= 0.5 * t;
    currentPos.y = mod(currentPos.y + 1.0, 2.0) - 1.0;
    return currentPos;
}

vec3 renderSnow(vec2 uv)
{
    vec3 fragColor = vec3(0.0);

    float t = iTime;
    for (float i = 0.0; i < NUM_SNOW; i++)
    {
        vec2 initPos = hash2d(i) * 2.0 - 1.0;
        vec2 pos = moveParticle(initPos, t);
        float brightness = 0.002;
        vec3 color = vec3(1.0);

        fragColor += renderParticle(uv, pos, brightness, color);
    }

    return fragColor;
}

void main()
{
    vec3 outputColor = renderSnow(vtx_pos.xy);

    vec2 uv = vec2(vtx_uv.x, -vtx_uv.y);

    frag_color = vec4(outputColor, 1.0);
}