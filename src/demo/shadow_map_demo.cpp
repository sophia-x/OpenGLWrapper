#include <gui/WindowManager.hpp>

#include <controllers/RotateCameraController.hpp>

#include <model/World.hpp>
#include <model/Light.hpp>
#include <model/SingleTextureModel.hpp>
#include <model/VertexTextureModel.hpp>
#include <model/Material.hpp>

#include <tools/Timer.hpp>

#include "Demo.hpp"

#define TITLE "Shadow Map Demo"
#define LIGHT_NAME "Light"
#define TEXTURE_NAME "Room_Texture"

void shadow_map_demo() {
	WindowManager &manager = WindowManager::getWindowManager();

	WindowManager::getWindowManager().addWindow(NAME, createWindow(WIDTH, HEIGHT, TITLE, NAME));
	manager.setCurrent(NAME);
	manager.turnGlewOn();
	manager.addCurrentController(new RotateCameraController(manager.getCurrentWindow(), vec3(0, 0, 15), 45.0f, 1.0f, 0.005f));
	opengl_init(BG_COLOR);

	World *world = new World();
	manager.addWorld(NAME, world);
	manager.setCurrentWorld(NAME);

	static const string SHADOW_MAP_NAME  = "Shadow_Map";
	init_shadow_map_simple_shader(world, "shaders/ShadowMapping_simple.vertexshader", "shaders/ShadowMapping_simple.fragmentshader", SHADOW_MAP_NAME);
	static const string DEPTH_NAME  = "Depth";
	init_depth_shader(world, "shaders/DepthRTT.vertexshader", "shaders/DepthRTT.fragmentshader", DEPTH_NAME);
	static const string PASS_THROUGH_NAME  = "PassThrough";
	init_passthrough_shader(world, "shaders/Passthrough.vertexshader", "shaders/Passthrough.fragmentshader", PASS_THROUGH_NAME);

	PointLight *light = new PointLight(vec3(0.5, 2, 2), vec3(1), 50.f);
	world->addLight(LIGHT_NAME, light);

	SingleTextureModel *model_ptr = new SingleTextureModel("models/room_thickwalls.obj", depth_map_set_up, 1024, depth_shader_set_up, DEPTH_NAME);
	model_ptr->addTexture(TEXTURE_NAME, "textures/room_thickwalls.DDS");
	model_ptr->setLightName(LIGHT_NAME);
	model_ptr->addInstance("Room", SingleModelInstance(SHADOW_MAP_NAME, TEXTURE_NAME, "", shadow_map_simple_set_up_shader));
	world->addModel("Shadow", model_ptr);

	vector<GLfloat> data = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
	vector<GLfloat> uvs(data.size() / 3 * 2);
	for (size_t i = 0; i < uvs.size() / 2; i++) {
		uvs[2 * i] = (data[3 * i] + 1) / 2;
		uvs[2 * i + 1] = (data[3 * i + 1] + 1) / 2;
	}
	VertexTextureModel* vertex_texture_model_ptr = new VertexTextureModel(data, uvs, GL_TRIANGLES);
	vertex_texture_model_ptr->addTexture("Depth_Texture", model_ptr->getBufferTexture());
	vertex_texture_model_ptr->addInstance("Overview", VertexTextureInstance(PASS_THROUGH_NAME, "Depth_Texture", passthrough_shader_set_up));
	vertex_texture_model_ptr->setBorder(vector<GLint> {0, 0, 256, 256});
	world->addModel("Overview", vertex_texture_model_ptr);

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		manager.getCurrentController().control(delta);

		world->update_draw(delta);
	}
}