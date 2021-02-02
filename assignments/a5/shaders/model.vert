#version 330 core

// Add parts 1a-d first, then implement part 2a

layout (std140) uniform camera
{
	mat4 projection;
	mat4 view;
	mat4 pvm;
	mat4 ortho;
	vec4 position;
};

uniform mat4 model=mat4(1.0f);

layout (location=0) in vec4 pos;
layout (location=2) in vec4 normal;
layout (location=3) in vec4 uv;
layout (location=4) in vec4 tangent;

out vec3 vtx_position;
out vec3 vtx_normal;

///////////// Part 1a /////////////////////
/* Create a function that takes in an xy coordinate and returns a 'random' 2d vector. (There is no right answer)
   Feel free to find a hash function online. Use the commented function to check your result */
vec2 hash2(vec2 v){
	vec2 rand = vec2(0,0);
	
	// Your implementation starts here

	// example hash function
	// rand  = 50.0 * 1.05 * fract(v * 0.3183099 + vec2(0.71, 0.113));
    // rand = -1.0 + 2 * 1.05 * fract(rand.x * rand.y * (rand.x + rand.y) * rand);
	
	// Your implementation ends here

	return rand;
}

///////////// Part 1b /////////////////////
/* Complete the function 'mix', which takes in 2 floats and returns the linear interpolated value at m (a float between 0 and 1) 
	if m is 0, mix should return x. if m is 1, mix should return y */
float mix(float x, float y, float m){
	float n = 0;
	// Your implementation starts here

	// Your implementation ends here
	return n;
}

///////////// Part 1c /////////////////////
/*  Using i, f, and m, compute the perlin noise at point v */
float perlin_noise(vec2 v) {
    float noise = 0;
	vec2 i = floor(v);
    vec2 f = fract(v);
    vec2 m = f*f*(3.0-2.0*f);
	// Your implementation starts here

	// Your implementation ends here
	return noise;
}

///////////// Part 1d /////////////////////
/*  Given a point v and an int num, compute the perlin noise octave for point v with octave num
	num will be greater than 0 */
float noiseOctave(vec2 v, int num){
	float sum = 0;
	// Your implementation starts here

	// Your implementation ends here
	return sum;
}

///////////// Part 2a /////////////////////
/* create a function that takes in a 2D point and returns a height using perlin noise 
    There is no right answer. Think about what functions will create what shapes.
    If you want steep mountains with flat tops, use a function like sqrt(noiseOctave(v,num)). 
    If you want jagged mountains, use a function like e^(noiseOctave(v,num))
    You can also add functions on top of each other and change the frequency of the noise
    by multiplying v by some value other than 1*/
float height(vec2 v){
    float h = 0;
	// Your implementation starts here

	// Your implementation ends here
	return h;
}

void main()												
{
	vtx_position = (model * vec4(pos.xy, height(pos.xy), 1)).xyz;
    gl_Position = pvm * vec4(vtx_position,1);
}