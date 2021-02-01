//#####################################################################
// Main
// Dartmouth COSC 77/177 Computer Graphics, starter code
// Contact: Bo Zhu (bo.zhu@dartmouth.edu)
//#####################################################################
#include <iostream>
#include <random>
#include <unordered_set>
#include "Common.h"
#include "Driver.h"
#include "OpenGLMesh.h"
#include "OpenGLCommon.h"
#include "OpenGLWindow.h"
#include "OpenGLViewer.h"
#include "TinyObjLoader.h"

#ifndef __Main_cpp__
#define __Main_cpp__

const std::string my_vertex_shader=To_String(
/*default camera matrices. do not modify.*/
~include version;
layout (std140) uniform camera
{
	mat4 projection;
	mat4 view;
	mat4 pvm;
	mat4 ortho;
	vec4 position;
};

layout (location=0) in vec4 pos;
layout (location=1) in vec4 v_color;

out vec4 vtx_color;

void main()												
{
	gl_Position=pvm*vec4(pos.xyz,1.f);
	vtx_color=vec4(pos.xyz,1.f);
}														
);

const std::string my_fragment_shader=To_String(
~include version;
in vec4 vtx_color;
out vec4 frag_color;
void main()								
{										
    frag_color=vtx_color;
}										
);

class ShaderDriver : public Driver, public OpenGLViewer
{using Base=Driver;
	OpenGLTriangleMesh* opengl_tri_mesh=nullptr;						////mesh
	TriangleMesh<3>* tri_mesh=nullptr;
	OpenGLSegmentMesh* opengl_normals=nullptr;							////normals

	bool use_obj_mesh=false;											////flag for use obj, set it to be true if you want to load an obj mesh
	std::string obj_mesh_name="cap.obj";								////obj file name

public:
	virtual void Initialize()
	{
		OpenGLViewer::Initialize();
	}

	void Load_Mesh()
	{
		if(use_obj_mesh){
			Array<std::shared_ptr<TriangleMesh<3> > > meshes;
			Obj::Read_From_Obj_File(obj_mesh_name,meshes);
			*tri_mesh=*meshes[0];
			std::cout<<"load tri_mesh, #vtx: "<<tri_mesh->Vertices().size()<<", #ele: "<<tri_mesh->Elements().size()<<std::endl;		
		}
		else{
			Initialize_Sphere_Mesh((real).5,tri_mesh,3);
		}	
	}

	virtual void Initialize_Data()
	{
		////initialize shader
		OpenGLShaderLibrary::Instance()->Add_Shader(my_vertex_shader,my_fragment_shader,"a2_shader");

		////initialize tri mesh
		opengl_tri_mesh=Add_Interactive_Object<OpenGLTriangleMesh>();
		opengl_tri_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("a2_shader"));
		tri_mesh=&opengl_tri_mesh->mesh;

		Load_Mesh();

		Set_Polygon_Mode(opengl_tri_mesh,PolygonMode::Fill);
		Set_Shading_Mode(opengl_tri_mesh,ShadingMode::A2);
		Set_Color(opengl_tri_mesh,OpenGLColor(.3f,.3f,.3f,1.f));
		opengl_tri_mesh->Set_Data_Refreshed();
		opengl_tri_mesh->Initialize();
	}

	////synchronize data to visualization
	void Sync_Simulation_And_Visualization_Data()
	{
		opengl_tri_mesh->Set_Data_Refreshed();
		opengl_tri_mesh->Initialize();
	}

	////update simulation and visualization for each time step
	virtual void Toggle_Next_Frame()
	{
		Sync_Simulation_And_Visualization_Data();
		OpenGLViewer::Toggle_Next_Frame();
	}

	virtual void Run()
	{
		OpenGLViewer::Run();
	}

	////Keyboard interaction
	virtual void Initialize_Common_Callback_Keys()
	{
		OpenGLViewer::Initialize_Common_Callback_Keys();
	}
};

int main(int argc,char* argv[])
{
	ShaderDriver driver;
	driver.Initialize();
	driver.Run();	
}

#endif