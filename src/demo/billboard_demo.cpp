#include <gui/WindowManager.hpp>

#include <controllers/RotateCameraController.hpp>

#include <model/World.hpp>
#include <model/VertexColorModel.hpp>
#include <model/VertexTextureModel.hpp>

#include <tools/Timer.hpp>

#include "Demo.hpp"

#define TITLE "Billboard"
#define TEXTURE_NAME "TEXTURE"

void billboard_demo() {
	WindowManager &manager = WindowManager::getWindowManager();

	WindowManager::getWindowManager().addWindow(NAME, createWindow(WIDTH, HEIGHT, TITLE, NAME));
	manager.setCurrent(NAME);
	manager.turnGlewOn();
	manager.addCurrentController(new RotateCameraController(manager.getCurrentWindow(), vec3(0, 0, 5), 45.0f, 1.0f, 0.005f));
	opengl_init(BG_COLOR);

	World *world = new World();
	manager.addWorld(NAME, world);
	manager.setCurrentWorld(NAME);

	const static string VERTEX_COLOR_NAME = "Vertex_Color_Shader";
	init_vertex_color_shader(world, "shaders/TransformVertexShader.vertexshader", "shaders/ColorFragmentShader.fragmentshader", VERTEX_COLOR_NAME);
	const static string BILLBOARD_2D_NAME = "Billboard_2D_Shader";
	init_billboard_less_shader(world, "shaders/Billboard_2D.vertexshader", "shaders/Billboard.fragmentshader", BILLBOARD_2D_NAME);
	// const static string BILLBOARD_FIX_NAME = "Billboard_fix_Shader";
	// init_billboard_less_shader(world, "shaders/Billboard_fix.vertexshader", "shaders/Billboard.fragmentshader", BILLBOARD_FIX_NAME);
	// const static string BILLBOARD_3D_NAME = "Billboard_3D_Shader";
	// init_billboard_more_shader(world, "shaders/Billboard_3D.vertexshader", "shaders/Billboard.fragmentshader", BILLBOARD_3D_NAME);
	// const static string BILLBOARD_VERT_NAME = "Billboard_vert_Shader";
	// init_billboard_more_shader(world, "shaders/Billboard_vert.vertexshader", "shaders/Billboard.fragmentshader", BILLBOARD_VERT_NAME);

	vector<GLfloat> p_vertex_data = { -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f};

	vector<vec3> vertex_data(p_vertex_data.size() / 3);
	for (size_t i = 0; i < vertex_data.size(); i++) {
		vertex_data[i] = vec3(p_vertex_data[3 * i], p_vertex_data[3 * i + 1], p_vertex_data[3 * i + 2]);
	}
	vector<GLfloat> color_data = { 0.583f,  0.771f,  0.014f, 0.609f,  0.115f,  0.436f, 0.327f,  0.483f,  0.844f, 0.822f,  0.569f,  0.201f, 0.435f,  0.602f,  0.223f, 0.310f,  0.747f,  0.185f, 0.597f,  0.770f,  0.761f, 0.559f,  0.436f,  0.730f, 0.359f,  0.583f,  0.152f, 0.483f,  0.596f,  0.789f, 0.559f,  0.861f,  0.639f, 0.195f,  0.548f,  0.859f, 0.014f,  0.184f,  0.576f, 0.771f,  0.328f,  0.970f, 0.406f,  0.615f,  0.116f, 0.676f,  0.977f,  0.133f, 0.971f,  0.572f,  0.833f, 0.140f,  0.616f,  0.489f, 0.997f,  0.513f,  0.064f, 0.945f,  0.719f,  0.592f, 0.543f,  0.021f,  0.978f, 0.279f,  0.317f,  0.505f, 0.167f,  0.620f,  0.077f, 0.347f,  0.857f,  0.137f, 0.055f,  0.953f,  0.042f, 0.714f,  0.505f,  0.345f, 0.783f,  0.290f,  0.734f, 0.722f,  0.645f,  0.174f, 0.302f,  0.455f,  0.848f, 0.225f,  0.587f,  0.040f, 0.517f,  0.713f,  0.338f, 0.053f,  0.959f,  0.120f, 0.393f,  0.621f,  0.362f, 0.673f,  0.211f,  0.457f, 0.820f,  0.883f,  0.371f, 0.982f,  0.099f,  0.879f};

	VertexColorModel* vertex_color_model_ptr = new VertexColorModel(vertex_data, color_data, 3, GL_TRIANGLES);
	vertex_color_model_ptr->addInstance("CUBE", Instance(shared_ptr<Base>{new Base(vec3(), quat(), vec3(0.2f))}, VERTEX_COLOR_NAME, vertex_color_set_up_shader));
	world->addModel("CUBE", vertex_color_model_ptr);

	vector<GLfloat> data = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	};
	vector<GLfloat> uvs(data.size() / 3 * 2);
	for (size_t i = 0; i < uvs.size() / 2; i ++) {
		uvs[2 * i] = data[3 * i] + 0.5;
		uvs[2 * i + 1] = data[3 * i + 1] + 0.5;
	}

	VertexTextureModel* vertex_texture_model_ptr = new VertexTextureModel(data, uvs, GL_TRIANGLE_STRIP);
	vertex_texture_model_ptr->addTexture(TEXTURE_NAME, "textures/billboard.DDS");
	vertex_texture_model_ptr->addInstance("Billboard", VertexTextureInstance(shared_ptr<Base>{new Base(vec3(0, 0.5, 0), quat(),
	                                      vec3(1, 0.125, 0))}, BILLBOARD_2D_NAME, TEXTURE_NAME, billboard_less_set_up_shader));
	// vertex_texture_model_ptr->addInstance("Billboard", VertexTextureInstance(shared_ptr<Base>{new Base(vec3(0, 0.5, 0), quat(),
	//                                       vec3(0.2, 0.05, 0))}, BILLBOARD_FIX_NAME, TEXTURE_NAME, billboard_less_set_up_shader));
	// vertex_texture_model_ptr->addInstance("Billboard", VertexTextureInstance(shared_ptr<Base>{new Base(vec3(0, 0.5, 0), quat(),
	//                                       vec3(1, 0.125, 0))}, BILLBOARD_3D_NAME, TEXTURE_NAME, billboard_more_set_up_shader));
	// vertex_texture_model_ptr->addInstance("Billboard", VertexTextureInstance(shared_ptr<Base>{new Base(vec3(0, 0.5, 0), quat(),
	//                                       vec3(1, 0.125, 0))}, BILLBOARD_VERT_NAME, TEXTURE_NAME, billboard_more_set_up_shader));
	world->addModel("Billboard", vertex_texture_model_ptr);

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		manager.getCurrentController().control(delta);

		float life_level = sin(timer.getCurrentTime()) * 0.1f + 0.7f;
		vertex_texture_model_ptr->setLifeLevel("Billboard", life_level);

		world->update_draw(delta);
	}
}