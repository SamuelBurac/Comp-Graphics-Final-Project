//#####################################################################
// Main
// Dartmouth COSC 77/177 Computer Graphics, starter code
// Contact: Bo Zhu (bo.zhu@dartmouth.edu)
//#####################################################################
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
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

class ShaderDriver : public Driver, public OpenGLViewer
{using Base=Driver;
	std::vector<OpenGLTriangleMesh*> mesh_object_array;						////mesh objects

public:
	virtual void Initialize()
	{
		draw_bk=true;
		OpenGLViewer::Initialize();
	}

	int Add_Obj_Mesh_Object(std::string obj_file_name)
	{
		auto mesh_obj=Add_Interactive_Object<OpenGLTriangleMesh>();

		Array<std::shared_ptr<TriangleMesh<3> > > meshes;
		Obj::Read_From_Obj_File(obj_file_name,meshes);
		mesh_obj->mesh=*meshes[0];
		std::cout<<"load tri_mesh from obj file, #vtx: "<<mesh_obj->mesh.Vertices().size()<<", #ele: "<<mesh_obj->mesh.Elements().size()<<std::endl;		

		mesh_object_array.push_back(mesh_obj);
		return (int)mesh_object_array.size()-1;
	}

	int Add_Sphere_Object(const double radius=1.)
	{
		auto mesh_obj=Add_Interactive_Object<OpenGLTriangleMesh>();

		Initialize_Sphere_Mesh(radius,&mesh_obj->mesh,3);		////add a sphere with radius=1. if the obj file name is not specified

		mesh_object_array.push_back(mesh_obj);
		return (int)mesh_object_array.size()-1;
	}


	void Translate_Vertex_Position_For_Mesh_Object(OpenGLTriangleMesh* obj,const Vector3& translate)
	{
		std::vector<Vector3>& vertices=obj->mesh.Vertices();		
		for(auto& v:vertices){
			v+=translate;
		}
	}

	void Update_Vertex_Color_And_Normal_For_Mesh_Object(OpenGLTriangleMesh* obj)
	{
		int vn=(int)obj->mesh.Vertices().size();					////number of vertices of a mesh
		std::vector<Vector3>& vertices=obj->mesh.Vertices();		////you might find this array useful
		std::vector<Vector3i>& elements=obj->mesh.Elements();		////you might find this array also useful

		std::vector<Vector4f>& vtx_color=obj->vtx_color;
		vtx_color.resize(vn);
		std::fill(vtx_color.begin(),vtx_color.end(),Vector4f::Zero());

		////TODO: update the color for each vertex.
		////NOTICE: This code updates the vertex color array on the CPU end. The array will then be sent to GPU and read it the vertex shader as v_color.
		////uncomment this sample implementation to see some color!
		for(int i=0;i<vn;i++){
			vtx_color[i]=Vector4f(0.,1.,0.,1.);	////specify color for each vertex
		}

		std::vector<Vector3>& vtx_normal=obj->vtx_normal;
		vtx_normal.resize(vn);
		std::fill(vtx_normal.begin(),vtx_normal.end(),Vector3::Zero());

		////TODO: update the normal for each vertex
		////NOTICE: This code updates the vertex normal array on the CPU end. The array will then be sent to GPU and read it the vertex shader as normal.
		////uncomment this sample implementation to see some color!
		////This is a default implementation of vertex normal that works for a sphere centered around the origin only.
		for(int i=0;i<vn;i++){
			vtx_normal[i]=Vector3(vertices[i][0],vertices[i][1],vertices[i][2]);
		}	
	}

	virtual void Initialize_Data()
	{
		////This is the first mesh in the scene.
		{
			int obj_idx=Add_Sphere_Object();	////add a sphere
			auto obj=mesh_object_array[obj_idx];
			Update_Vertex_Color_And_Normal_For_Mesh_Object(obj);		
		}

		////If you want to put multiple objects in the scene, uncomment this block.
		//{
		//	int obj_idx=Add_Sphere_Object();	////add a sphere
		//	auto obj=mesh_object_array[obj_idx];
		//	Translate_Vertex_Position_For_Mesh_Object(obj,Vector3::Unit(0)*3.);
		//	Update_Vertex_Color_And_Normal_For_Mesh_Object(obj);		
		//}

		////initialize shader
		std::string vertex_shader_file_name="helloworld.vert";
		std::string fragment_shader_file_name="helloworld.frag";
		OpenGLShaderLibrary::Instance()->Add_Shader_From_File(vertex_shader_file_name,fragment_shader_file_name,"a2_shader");

		for(auto& mesh_obj: mesh_object_array){
			mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("a2_shader"));
			Set_Polygon_Mode(mesh_obj,PolygonMode::Fill);
			Set_Shading_Mode(mesh_obj,ShadingMode::A2);
			mesh_obj->Set_Data_Refreshed();
			mesh_obj->Initialize();	
		}
	}

	////update simulation and visualization for each time step
	virtual void Toggle_Next_Frame()
	{
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