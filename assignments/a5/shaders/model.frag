#version 330 core

layout (std140) uniform camera
{
	mat4 projection;
	mat4 view;
	mat4 pvm;
	mat4 ortho;
	vec4 position;
};

struct light
{
	ivec4 att;
	vec4 pos;
	vec4 dir;
	vec4 amb;
	vec4 dif;
	vec4 spec;
	vec4 atten;
	vec4 r;
};

layout (std140) uniform lights
{
	vec4 amb;		// ambient color
	ivec4 lt_att;	// lt_att[0]: number of lights
	light lt[2];	// Values for each light
};

in vec3 vtx_normal;
in vec3 vtx_position;
out vec4 frag_color;

vec3 phong(int i, vec4 baseColor, vec3 n, vec3 v, float shininess, float specularStrength);

vec3 lighting(vec4 baseColor, vec3 n, vec3 v, float shininess, float specularStrength) {
	vec3 color = vec3(0);
	for(int i=0;i<lt_att[0];i++) {
		color += phong(i,baseColor,n,v,shininess,specularStrength);
    }
	return color;
}

vec3 phong(int i, vec4 baseColor, vec3 n, vec3 v, float shininess, float specularStrength) {

	vec3 lightDir = normalize(-lt[i].dir.xyz);
	vec3 lightCol = lt[i].dif.rgb;
	vec3 ambient_color = amb.rgb;
	
	vec3 phong_lighting = vec3(0);
	vec3 r = normalize(-lightDir + 2 * (dot(lightDir,n))*n);
	vec3 ambient = ambient_color;
	vec3 diffuse = lightCol* baseColor.rgb * max(0, dot(n, lightDir));
	vec3 specular = lightCol * specularStrength * pow(max(0, dot(r, v)), shininess);
	phong_lighting = ambient + diffuse + specular;
	return phong_lighting;
}

vec3 compute_vertex_to_eye() {
	vec3 vertex_to_eye = vec3(0);
	vertex_to_eye = normalize(position.xyz - vtx_position);
	return vertex_to_eye;
}

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

///////////// Part 2b /////////////////////
/* compute the normal vector at v by find the points d to the left/right and d forward/backward 
    and using a cross product. Be sure to normalize the result */
vec3 compute_normal(vec2 v, float d){
	vec3 normal_vector = vec3(0,0,0);
	vec3 v1 = vec3(v.x + d, v.y, height(vec2(v.x + d, v.y)));
	vec3 v2 = vec3(v.x - d, v.y, height(vec2(v.x - d, v.y)));
	vec3 v3 = vec3(v.x, v.y + d, height(vec2(v.x, v.y + d)));
	vec3 v4 = vec3(v.x, v.y - d, height(vec2(v.x, v.y - d)));
	// Your implementation starts here

	// Your implementation ends here
	return normal_vector;
}

///////////// Part 2c /////////////////////
/* complete the get_color function by setting emissiveColor using some function of height/normal vector/noise */
vec3 get_color(vec2 v){
	vec3 norm = compute_normal(v, 0.01);
    vec3 vertex_to_eye = compute_vertex_to_eye();
	float shininess = 90;
    float specularStrength = 0.0;
    vec4 baseColor = vec4(0.8, 0.8, 0.8, 1); 
	vec3 emissiveColor = vec3(0,0,0);
	// Your implementation starts here

	// Your implementation ends here
    return emissiveColor * lighting(baseColor, norm, vertex_to_eye, shininess, specularStrength);
}

void main()
{
	frag_color = vec4(get_color(vtx_position.xy),1.f);
}