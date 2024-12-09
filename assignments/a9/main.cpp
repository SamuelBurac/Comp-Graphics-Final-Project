#include "Common.h"
#include "OpenGLCommon.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLBgEffect.h"
#include "OpenGLMesh.h"
#include "OpenGLViewer.h"
#include "OpenGLWindow.h"
#include "TinyObjLoader.h"
#include "OpenGLSkybox.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>
#include <string>

#ifndef __Main_cpp__
#define __Main_cpp__

#ifdef __APPLE__
#define CLOCKS_PER_SEC 100000
#endif

class MyDriver : public OpenGLViewer
{
    std::vector<OpenGLTriangleMesh *> mesh_object_array;
    OpenGLBgEffect *bgEffect = nullptr;
    OpenGLSkybox *skybox = nullptr;
    clock_t startTime;

public:
    virtual void Initialize()
    {
        draw_axes = false;
        startTime = clock();
        OpenGLViewer::Initialize();
    }

    std::pair<std::vector<Vector3>, std::vector<Vector3i>> generate_l_system(int iterations, vec3 p)
    {
        float segment_length = 0.1f;
        float segment_width = 0.03f;

        std::vector<Vector3> vertices;
        std::vector<Vector3i> elements;

        Vector3 pos = Vector3(p.x, p.y, p.z);
        Vector3 dir = Vector3(0, 1, 0);

        float angle = 25.0 * M_PI / 180;

        std::string axiom = "VZFFF"; // VZFFF
        std::string command = "";

        // create command via iteration on first axiom
        for (int i = 0; i < iterations; i++)
        {
            command = "";

            for (char c : axiom)
            {
                if (c == 'V')
                    command += "[+++W][---W]F[++W][--W]F[+W][-W]YV";
                else if (c == 'X')
                    command += "-W[+X]Z";
                else if (c == 'W')
                    command += "+X[-W]Z";
                else if (c == 'Y')
                    command += "Y[++W][--W]Z";
                else if (c == 'Z')
                    command += "[-FFF][+FFF]F";
                else
                    command += c;
            }

            axiom = command;
        }

        // create vertices and triangles
        std::stack<std::pair<Vector3, Vector3>> buildStack; // hold direction and position
        int vertex_count = 0;

        for (char c : command)
        {
            switch (c)
            {
            case '-':
                dir = Vector3(dir.x() * cos(-angle) - dir.y() * sin(-angle),
                              dir.x() * sin(-angle) + dir.y() * cos(-angle), dir.z());
                break;
            case '+':
                dir = Vector3(dir.x() * cos(angle) - dir.y() * sin(angle),
                              dir.x() * sin(angle) + dir.y() * cos(angle), dir.z());
                break;
            case '[':
                buildStack.push(std::make_pair(dir, pos));
                break;
            case ']':
                {
                    std::pair<Vector3, Vector3> dp = buildStack.top();
                    buildStack.pop();
                    dir = dp.first;
                    pos = dp.second;
                }

                break;
            default:

                if (c == 'F' || c == 'X' || c == 'W' || c == 'Y' || c == 'V' || c == 'Z')
                {
                    

                    // Create rectangle vertices
                    Vector3 v0 = pos;
                    Vector3 v1 = Vector3(pos.x() + segment_width, pos.y(),pos.z());

                    Vector3 next_pos = pos + dir * segment_length;
                    
                    Vector3 v2 = next_pos;
                    Vector3 v3 = Vector3(next_pos.x() + segment_width, next_pos.y(),next_pos.z());


                    vertices.push_back(v0);
                    vertices.push_back(v1);
                    vertices.push_back(v2);
                    vertices.push_back(v3);

                    Vector3i tri1 = Vector3i(vertex_count, vertex_count + 1, vertex_count + 2);
                    Vector3i tri2 = Vector3i(vertex_count + 1, vertex_count + 3, vertex_count + 2);

                    elements.push_back(tri1);
                    elements.push_back(tri2);

                    pos = next_pos;
                    vertex_count += 4;
                }

                break;
            }
        }

        return {vertices, elements};
    }

    void placeTree(vec3 p)
    {
        // trunk
        {
            std::vector<Vector3> vertices = {Vector3(p.x, p.y - 1, p.z), Vector3(p.x + 0.3, p.y - 1, p.z), Vector3(p.x, p.y + 3, p.z), Vector3(p.x + 0.2, p.y + 3, p.z)};
            std::vector<Vector3i> elements = {Vector3i(0, 1, 2), Vector3i(1, 3, 2)};
            auto trunk = Add_Tri_Mesh_Object(vertices, elements);
            // ! you can also set uvs
            trunk->mesh.Uvs() = {Vector2(0, 0), Vector2(1, 0), Vector2(1, 1), Vector2(0, 1)};

            Matrix4f t;
            t << 0.75, 0, 0, -0.070,
                0, 0.75, 0, 0,
                0, 0, 0.75, -0.01,
                0, 0, 0, 1;

            trunk->Set_Model_Matrix(t);

            trunk->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("brown"));

            trunk->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic"));
        }

        // Pine needles
        {
            auto [vertices, elements] = generate_l_system(7, p);
            auto obj = Add_Tri_Mesh_Object(vertices, elements);

            Matrix4f t;
            t << 0.75, 0, 0, 0,
                0, 0.75, 0, 0,
                0, 0, 0.75, 0,
                0, 0, 0, 1;

            obj->Set_Model_Matrix(t);

            // Add material properties
            obj->Set_Ka(Vector3f(0.0f, 0.0f, 1.0f));
            obj->Set_Kd(Vector3f(0.0f, 0.0f, 1.0f));
            obj->Set_Ks(Vector3f(0.0f, 0.0f, 1.0f));
            obj->Set_Shininess(32.0f);

            obj->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("green"));
            obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic"));
        }
    }

    virtual void Initialize_Data()
    {
        //// Load all the shaders you need for the scene
        //// In the function call of Add_Shader_From_File(), we specify three names:
        //// (1) vertex shader file name
        //// (2) fragment shader file name
        //// (3) shader name used in the shader library
        //// When we bind a shader to an object, we implement it as follows:
        //// object->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("shader_name"));
        //// Here "shader_name" needs to be one of the shader names you created previously with Add_Shader_From_File()

        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/basic.frag", "basic");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/environment.frag", "environment");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/stars.vert", "shaders/stars.frag", "stars");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/alphablend.frag", "blend");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/billboard.vert", "shaders/alphablend.frag", "billboard");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/terrain.vert", "shaders/terrain.frag", "terrain");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/skybox.vert", "shaders/skybox.frag", "skybox");

        //// Load all the textures you need for the scene
        //// In the function call of Add_Shader_From_File(), we specify two names:
        //// (1) the texture's file name
        //// (2) the texture used in the texture library
        //// When we bind a texture to an object, we implement it as follows:
        //// object->Add_Texture("tex_sampler", OpenGLTextureLibrary::Get_Texture("tex_name"));
        //// Here "tex_sampler" is the name of the texture sampler2D you used in your shader, and
        //// "tex_name" needs to be one of the texture names you created previously with Add_Texture_From_File()

        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/earth_color.png", "sphere_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/earth_normal.png", "sphere_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/bunny_color.jpg", "bunny_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/bunny_normal.png", "bunny_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/window.png", "window_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/buzz_color.png", "buzz_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/star.png", "star_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/Elk_color.png", "Elk_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/Elk_normal.png", "Elk_Normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/brown.png", "brown");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/green.png", "green");

        //// Add all the lights you need for the scene (no more than 4 lights)
        //// The four parameters are position, ambient, diffuse, and specular.
        //// The lights you declared here will be synchronized to all shaders in uniform lights.
        //// You may access these lights using lt[0].pos, lt[1].amb, lt[1].dif, etc.
        //// You can also create your own lights by directly declaring them in a shader without using Add_Light().
        //// Here we declared three default lights for you. Feel free to add/delete/change them at your will.

        opengl_window->Add_Light(Vector3f(3, 1, 3), Vector3f(0.1, 0.1, 0.1), Vector3f(1, 1, 1), Vector3f(0.5, 0.5, 0.5));
        opengl_window->Add_Light(Vector3f(0, 0, -5), Vector3f(0.1, 0.1, 0.1), Vector3f(0.9, 0.9, 0.9), Vector3f(0.5, 0.5, 0.5));
        opengl_window->Add_Light(Vector3f(-5, 1, 3), Vector3f(0.1, 0.1, 0.1), Vector3f(0.9, 0.9, 0.9), Vector3f(0.5, 0.5, 0.5));

        //// Add the background / environment
        //// Here we provide you with four default options to create the background of your scene:
        //// (1) Gradient color (like A1 and A2; if you want a simple background, use this one)
        //// (2) Programmable Canvas (like A7 and A8; if you consider implementing noise or particles for the background, use this one)
        //// (3) Sky box (cubemap; if you want to load six background images for a skybox, use this one)
        //// (4) Sky sphere (if you want to implement a sky sphere, enlarge the size of the sphere to make it colver the entire scene and update its shaders for texture colors)
        //// By default, Option (2) (Buzz stars) is turned on, and all the other three are commented out.

        //// Background Option (1): Gradient color

        {
            auto bg = Add_Interactive_Object<OpenGLBackground>();
            bg->Set_Color(OpenGLColor(0.3f, 0.3f, 0.3f, 1.f), OpenGLColor(0.6f, 0.6f, .9f, 1.f));
            bg->Initialize();
        }

        //// Background Option (2): Programmable Canvas
        //// By default, we load a GT buzz + a number of stars
        // {
        //     bgEffect = Add_Interactive_Object<OpenGLBgEffect>();
        //     bgEffect->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("stars"));
        //     bgEffect->Add_Texture("tex_buzz", OpenGLTextureLibrary::Get_Texture("buzz_color")); // bgEffect can also Add_Texture
        //     bgEffect->Initialize();
        // }

        {
            //// create object by reading an obj mesh
            auto elkOne = Add_Obj_Mesh_Object("obj/elk.obj");

            //// set object's transform
            Matrix4f t;
            t << 0, 0, 0.5, -2,
                0, 0.5, 0, -1,
                0.5, 0, 0, 1.8,
                0, 0, 0, 1;
            elkOne->Set_Model_Matrix(t);

            //// set object's material
            elkOne->Set_Ka(Vector3f(0.1, 0.1, 0.1));
            elkOne->Set_Kd(Vector3f(0.7, 0.7, 0.7));
            elkOne->Set_Ks(Vector3f(2, 2, 2));
            elkOne->Set_Shininess(128);

            //// bind texture to object
            elkOne->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("Elk_color"));
            elkOne->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("Elk_Normal"));

            //// bind shader to object
            elkOne->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic"));
        }

        //// Here we load a elk object with the basic shader to show how to add an object into the scene
        {
            //// create object by reading an obj mesh
            auto elkTwo = Add_Obj_Mesh_Object("obj/elk.obj");

            //// set object's transform
            Matrix4f t;
            t << 0, 0, 0.55, -1.5,
                0, 0.55, 0, -0.8,
                0.55, 0, 0, 0.4,
                0, 0, 0, 1;
            elkTwo->Set_Model_Matrix(t);

            //// set object's material
            elkTwo->Set_Ka(Vector3f(0.1, 0.1, 0.1));
            elkTwo->Set_Kd(Vector3f(0.7, 0.7, 0.7));
            elkTwo->Set_Ks(Vector3f(2, 2, 2));
            elkTwo->Set_Shininess(128);

            //// bind texture to object
            elkTwo->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("Elk_color"));
            elkTwo->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("Elk_Normal"));

            //// bind shader to object
            elkTwo->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic"));
        }

        //// Here we load a elk object with the basic shader to show how to add an object into the scene
        {
            //// create object by reading an obj mesh
            auto elkThree = Add_Obj_Mesh_Object("obj/elk.obj");

            //// set object's transform
            Matrix4f t;
            t << 0, 0, 0.6, -0.5,
                0, 0.6, 0, -1,
                0.6, 0, 0, 1,
                0, 0, 0, 1;
            elkThree->Set_Model_Matrix(t);

            //// set object's material
            elkThree->Set_Ka(Vector3f(0.1, 0.1, 0.1));
            elkThree->Set_Kd(Vector3f(0.7, 0.7, 0.7));
            elkThree->Set_Ks(Vector3f(2, 2, 2));
            elkThree->Set_Shininess(128);

            //// bind texture to object
            elkThree->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("Elk_color"));
            elkThree->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("Elk_Normal"));

            //// bind shader to object
            elkThree->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic"));
        }

        //// Here we show an example of adding a mesh with noise-terrain (A6)
        {
            //// create object by reading an obj mesh
            auto terrain = Add_Obj_Mesh_Object("obj/plane.obj");

            //// set object's transform
            Matrix4f r, s, t;
            r << 1, 0, 0, 0,
                0, 0.5, 0.67, 0,
                0, -0.67, 0.5, 0,
                0, 0, 0, 1;
            s << 0.5, 0, 0, 0,
                0, 0.5, 0, 0,
                0, 0, 0.5, 0,
                0, 0, 0, 1;
            t << 1, 0, 0, -2,
                0, 1, 0, 0.5,
                0, 0, 1, 0,
                0, 0, 0, 1,
                terrain->Set_Model_Matrix(t * s * r);

            //// set object's material
            terrain->Set_Ka(Vector3f(0.1f, 0.1f, 0.1f));
            terrain->Set_Kd(Vector3f(0.7f, 0.7f, 0.7f));
            terrain->Set_Ks(Vector3f(1, 1, 1));
            terrain->Set_Shininess(128.f);

            //// bind shader to object (we do not bind texture for this object because we create noise for texture)
            terrain->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("terrain"));
        }

        //// Here we create a mesh object with two triangle specified using a vertex array and a triangle array.
        //// This is an example showing how to create a mesh object without reading an .obj file.
        //// If you are creating your own L-system, you may use this function to visualize your mesh.

        placeTree(vec3(2, 0, 0));
        placeTree(vec3(-2.4, 0, -1.0));
        placeTree(vec3(3, 0, -2));
        placeTree(vec3(-3.4, 0, -2));

        //// This for-loop updates the rendering model for each object on the list
        for (auto &mesh_obj : mesh_object_array)
        {
            Set_Polygon_Mode(mesh_obj, PolygonMode::Fill);
            Set_Shading_Mode(mesh_obj, ShadingMode::TexAlpha);
            mesh_obj->Set_Data_Refreshed();
            mesh_obj->Initialize();
        }
        Toggle_Play();
    }

    //// add mesh object by reading an .obj file
    OpenGLTriangleMesh *Add_Obj_Mesh_Object(std::string obj_file_name)
    {
        auto mesh_obj = Add_Interactive_Object<OpenGLTriangleMesh>();
        Array<std::shared_ptr<TriangleMesh<3>>> meshes;
        // Obj::Read_From_Obj_File(obj_file_name, meshes);
        Obj::Read_From_Obj_File_Discrete_Triangles(obj_file_name, meshes);

        mesh_obj->mesh = *meshes[0];
        std::cout << "load tri_mesh from obj file, #vtx: " << mesh_obj->mesh.Vertices().size() << ", #ele: " << mesh_obj->mesh.Elements().size() << std::endl;

        mesh_object_array.push_back(mesh_obj);
        return mesh_obj;
    }

    //// add mesh object by reading an array of vertices and an array of elements
    OpenGLTriangleMesh *Add_Tri_Mesh_Object(const std::vector<Vector3> &vertices, const std::vector<Vector3i> &elements)
    {
        auto obj = Add_Interactive_Object<OpenGLTriangleMesh>();
        mesh_object_array.push_back(obj);
        // set up vertices and elements
        obj->mesh.Vertices() = vertices;
        obj->mesh.Elements() = elements;

        return obj;
    }

    //// Go to next frame
    virtual void Toggle_Next_Frame()
    {
        for (auto &mesh_obj : mesh_object_array)
            mesh_obj->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);

        if (bgEffect)
        {
            bgEffect->setResolution((float)Win_Width(), (float)Win_Height());
            bgEffect->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);
            bgEffect->setFrame(frame++);
        }

        if (skybox)
        {
            skybox->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);
        }

        OpenGLViewer::Toggle_Next_Frame();
    }

    virtual void Run()
    {
        OpenGLViewer::Run();
    }
};

int main(int argc, char *argv[])
{
    MyDriver driver;
    driver.Initialize();
    driver.Run();
}

#endif