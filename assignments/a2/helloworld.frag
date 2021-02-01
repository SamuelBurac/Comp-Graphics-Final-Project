/*Hello fragment shader!*/

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
out vec4 frag_color;	/*or use gl_FragColor*/

void main()							
{	
	gl_FragColor = vec4(vtx_color.rgb,1.f);
}	