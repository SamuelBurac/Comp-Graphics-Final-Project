/*This is your first fragment shader!*/

#version 330 core

/*default camera matrices. do not modify.*/
layout (std140) uniform camera
{
	mat4 projection;	/*camera's projection matrix*/
	mat4 view;			/*camera's view matrix*/
	mat4 pvm;			/*camera's projection*view*model matrix*/
	mat4 ortho;			/*camera's ortho projection matrix*/
	vec4 position;		/*camera's position in world space*/
};

/*input variables*/
in vec4 vtx_color;
in vec3 vtx_normal;
in vec3 vtx_position;

/*output variables*/
out vec4 frag_color;

/*hard-coded lighting parameters*/
const vec3 LightPosition = vec3(3, 1, 3);
const vec3 LightIntensity = vec3(20);
const vec3 ka = 0.1*vec3(1., 1., 1.);
const vec3 kd = 0.7*vec3(1., 1., 1.);
const vec3 ks = vec3(2.);
const float n = 400.0;

void main()							
{										
	vec3 _viewDir = normalize(position.xyz);
	vec3 _normal = normalize((vec4(vtx_normal, 0)).xyz);
	vec3 lightDir = LightPosition - vtx_position;
	float _lightDist = length(lightDir);
	vec3 _lightDir = normalize(lightDir);
	vec3 _localLight = LightIntensity / (_lightDist * _lightDist);
	vec3 ambColor = ka;
	vec3 difColor = kd * _localLight * max(0., dot(_lightDir, _normal));
	vec3 specColor = vec3(0);

	gl_FragColor = vec4(ambColor + difColor*vtx_color.rgb, 1); // Lambertian
	// gl_FragColor = vec4(ambColor + difColor, 1); // Phong
}	