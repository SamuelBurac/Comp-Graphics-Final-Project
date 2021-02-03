//#####################################################################
// Main
// Dartmouth COSC 77/277: Graphics, Assignment 5 starter code
// Modified from Evan Muscatel COSC 77 Tech Project from 19S
// Contact: Bo Zhu (bo.zhu@dartmouth.edu)
//#####################################################################


#include <iostream>

#include <random>
#include "Common.h"
#include "Driver.h"
#include "Particles.h"
#include "OpenGLMesh.h"
#include "OpenGLCommon.h"
#include "OpenGLWindow.h"
#include "OpenGLViewer.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLParticles.h"
#include "TinyObjLoader.h"

const int part = 2; //choose part

template<int d> class ToolkitDriver : public Driver, public OpenGLViewer
{
	using VectorD = Vector<real, d>; using VectorDi = Vector<int, d>; using Base = Driver;
	real dt = .02;
	OpenGLTriangleMesh* plane_mesh= nullptr;

public:
	virtual void Initialize()
	{
		OpenGLViewer::Initialize();
	}

	virtual void Initialize_Data()
	{
		OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/perlin.vert", "shaders/perlin.frag", "perlin");
		OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/model.vert", "shaders/model.frag", "model");
		Init_Plane();

		auto dir_light = OpenGLUbos::Add_Directional_Light(glm::vec3(-3.f, -3.f, -5.f));
		dir_light->dif = glm::vec4(1.0, 1.0, 1.0, 1.0);

		OpenGLUbos::Set_Ambient(glm::vec4(.01f, .01f, .02f, 1.f));
		OpenGLUbos::Update_Lights_Ubo();
	}

	void Init_Plane() {
		Array<std::shared_ptr<TriangleMesh<3> > > meshes;
		
		Obj::Read_From_Obj_File("models/plane.obj", meshes);
		plane_mesh = Add_Interactive_Object<OpenGLTriangleMesh>();
		plane_mesh->mesh = *meshes[0];
		plane_mesh->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		Set_Polygon_Mode(plane_mesh, PolygonMode::Fill);
		Set_Shading_Mode(plane_mesh, ShadingMode::Custom);
		plane_mesh->Set_Data_Refreshed();
		plane_mesh->Initialize();
		if(part == 1)
			plane_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("perlin"));
		else
			plane_mesh->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("model"));
	}

	void Sync_Simulation_And_Visualization_Data()
	{
	}

	////update simulation and visualization for each time step
	virtual void Toggle_Next_Frame()
	{
		Sync_Simulation_And_Visualization_Data();
		OpenGLViewer::Toggle_Next_Frame();
	}

	virtual void Run()
	{
		//OpenGLViewer::CameraPos = glm::vec3(-2.5, 0, .15);
		OpenGLViewer::Run();
	}

	////Keyboard interaction
	virtual void Initialize_Common_Callback_Keys()
	{
		OpenGLViewer::Initialize_Common_Callback_Keys();
	}

protected:
	////Helper function to convert a vector to 3d, for c++ template
	Vector3 V3(const Vector2& v2) { return Vector3(v2[0], v2[1], .0); }
	Vector3 V3(const Vector3& v3) { return v3; }
};


int main(int argc,char* argv[])
{
	int driver=1;

	switch(driver){
	case 1:{
		ToolkitDriver<3> driver;
		driver.Initialize();
		driver.Run();	
	}break;
	}
}

