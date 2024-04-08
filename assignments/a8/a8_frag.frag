#version 330 core

uniform vec2 iResolution;
uniform float iTime;
uniform int iFrame;
in vec2 fragCoord;              // screen space coordinate
out vec4 outputColor;           // output color
#define Time (iTime)

#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define Gravity 0.7             // gravity
#define NUM_STAR 30
#define NUM_EMISSION 30
#define NUM_FIREWORKS 10
#define DURATION 3.

const vec2 g = vec2(0.0, -Gravity);

/////////////////////////////////////////////////////
//// Hash functions
/////////////////////////////////////////////////////

//// This hash function takes input t and returns random float between 0 and 1
float hash1d(float t)
{
    t += 1.;
    return fract(sin(t * 674.3) * 453.2);
}

//// This hash function takes input t and returns random vec2 with each component between 0 and 1
vec2 hash2d(float t)
{
    t += 1.;
    float x = fract(sin(t * 674.3) * 453.2);
    float y = fract(sin((t + x) * 714.3) * 263.2);

    return vec2(x, y);
}

//// This hash function takes input t and returns random vec3 with each component between 0 and 1
vec3 hash3d(float t)
{
    t += 1.;
    float x = fract(sin(t * 674.3) * 453.2);
    float y = fract(sin((t + x) * 714.3) * 263.2);
    float z = fract(sin((t + y) * 134.3) * 534.2);

    return vec3(x, y, z);
}

//// This hash function takes input t and returns a random vec2 on a circle
vec2 hash2d_polar(float t)
{
    t += 1.;
    float a = fract(sin(t * 674.3) * 453.2) * TWO_PI;
    float d = fract(sin((t + a) * 714.3) * 263.2);
    return vec2(sin(a), cos(a)) * d;
}

/////////////////////////////////////////////////////
//// Step 1: render a single particle
//// 
/////////////////////////////////////////////////////

vec3 renderParticle(vec2 fragPos, vec2 particlePos, float brightness, vec3 color)
{
    vec3 frag_color = vec3(0.,0.,0.);

	/* your implementation starts */
     
	
    /* your implementation ends */

    return frag_color;
}

/////////////////////////////////////////////////////
//// Step 2: render the starry sky with multiple particles
//// 
/////////////////////////////////////////////////////

vec3 renderStars(vec2 fragPos)
{
    vec3 fragColor = vec3(0.01, 0.04, 0.3);
    float t = Time;

    for(float i = 0.; i < NUM_STAR; i++){
        vec2 pos = hash2d(i) * 2. - 1.;

        /* your implementation starts */

        
        /* your implementation ends */
    }

    return fragColor;
}

/////////////////////////////////////////////////////
//// Step 3: simulate the motion of a single particle by programming ballistic motion
//// 
/////////////////////////////////////////////////////

vec2 moveParticle(vec2 initPos, vec2 initVel, float t)
{
    vec2 currentPos = initPos;

    /* your implementation starts */


    /* your implementation ends */

    return currentPos;
}

vec3 simSingleParticle(vec2 fragPos, vec2 initPos, vec2 initVel, float t, float brightness, vec3 color)
{
    vec3 fragColor = vec3(0.0);

    vec2 particlePos = moveParticle(initPos, initVel, t);
    fragColor = renderParticle(fragPos, particlePos, brightness, color);

    return fragColor;
}

/////////////////////////////////////////////////////
//// Step 4: simulate firework
//// 
/////////////////////////////////////////////////////

vec3 simSingleFirework(vec2 fragPos, vec2 launchPos, vec2 launchVel, float t, vec3 color)
{
    vec3 fragColor = vec3(0.0);
    float emitTime = 1.5;

    if(t < emitTime){
        float brightness = .002;
        vec2 initPos = launchPos;
        vec2 initVel = launchVel;
        fragColor += simSingleParticle(fragPos, initPos, initVel, t, brightness, color);
    }
    else{
        float t2 = t - emitTime; // time since explosion
        vec2 initPos = moveParticle(launchPos, launchVel, emitTime);

        /* your implementation starts */


        /* your implementation ends */
    }

    fragColor *= 1.5; // make it brighter

    return fragColor;
}

vec3 renderFireworks(vec2 fragPos)
{
    vec3 fragColor = vec3(0.0);

    for(float i = 0.; i < NUM_FIREWORKS; i++){
        float lauchTime = i;
        float relTime = Time - lauchTime;
        float t = mod(relTime, DURATION);
        float idx = floor(relTime / DURATION);

        vec2 launchPos = vec2((hash1d(idx) * 2. - 1.) * 0.7, -0.5);
        vec2 launchVel = vec2(-launchPos.x * 0.66, hash1d(lauchTime + 1.) * 0.3 + .9);
        vec3 color = sin(40. * hash3d(lauchTime) * idx) * 0.25 + 0.75;

        fragColor += simSingleFirework(fragPos, launchPos, launchVel, t, color);
    }

    return fragColor;
}

void mainImage(out vec4 outputColor, in vec2 fragCoord)
{
    // fragPos's center is at the center of the screen, fragPos.y range is [-0.5, 0.5]
    vec2 fragPos = (fragCoord - .5 * iResolution.xy) / iResolution.y;

    vec3 fragColor = vec3(0.0);

    // step 1: render single particle
    {
        vec2 pos = vec2(0., 0.);
        float brightness = 0.005;
        vec3 color = vec3(0.15, 0.71, 0.92);
        fragColor = renderParticle(fragPos, pos, brightness, color);
    }

    
    // step 2: render starry sky
    {
        fragColor = renderStars(fragPos);
    }

    
    // step 3: simulate single particle
    {
        vec2 initPos = vec2(-0.5, -0.5);
        vec2 initVel = vec2(0.4, 1.);
        float t = mod(Time, DURATION);
        float brightnes = .005;
        vec3 color = vec3(0.15, 0.71, 0.92);
        fragColor = renderStars(fragPos) + simSingleParticle(fragPos, initPos, initVel, t, brightnes, color);
    }

    
    // step 4: simulate fireworks
    {
        fragColor = renderStars(fragPos) + renderFireworks(fragPos);
    }
    
    outputColor = vec4(fragColor, 1.0);
}

void main()
{
    mainImage(outputColor, fragCoord);
}
