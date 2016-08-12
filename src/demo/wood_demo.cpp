#include <gui/WindowManager.hpp>

#include <controllers/RotateCameraController.hpp>

#include <model/World.hpp>
#include <model/Light.hpp>
#include <model/SingleTextureModel.hpp>
#include <model/VertexTextureModel.hpp>
#include <model/Material.hpp>

#include <tools/Timer.hpp>

#include "Demo.hpp"

#define TITLE "Wood Demo"
#define LIGHT_NAME "Light"
#define TEXTURE_NAME "MONKEY_Texture"
#define MATERIAL_NAME "Material"

void wood_demo() {
	WindowManager &manager = WindowManager::getWindowManager();

	WindowManager::getWindowManager().addWindow(NAME, createWindow(WIDTH, HEIGHT, TITLE, NAME));
	manager.setCurrent(NAME);
	manager.turnGlewOn();
	manager.addCurrentController(new RotateCameraController(manager.getCurrentWindow(), vec3(0, 0, 5), 45.0f, 1.0f, 0.005f));
	opengl_init(BG_COLOR);

	World *world = new World();
	manager.addWorld(NAME, world);
	manager.setCurrentWorld(NAME);

	static const string STANDARD_SHADER_NAME  = "Standard_Shader";
	init_standard_shader(world, "shaders/StandardShading.vertexshader", "shaders/StandardShading.fragmentshader", STANDARD_SHADER_NAME);
	static const string DEPTH_NAME  = "Depth";
	init_depth_shader(world, "shaders/DepthRTT.vertexshader", "shaders/DepthRTT.fragmentshader", DEPTH_NAME);
	static const string WOOD_NAME  = "WoodName";
	init_wood_shader(world, "shaders/Passthrough.vertexshader", "shaders/WobblyTexture.fragmentshader", WOOD_NAME);

	PointLight *light = new PointLight(vec3(4), vec3(1), 1.0f);
	world->addLight(LIGHT_NAME, light);

	PhoneMaterial *material = new PhoneMaterial(vec3(0.1f), vec3(0.3f), 5);
	world->addMaterial(MATERIAL_NAME, material);

	SingleTextureModel *model_ptr = new SingleTextureModel("models/monkey.obj", wood_map_set_up, vec2(WIDTH, HEIGHT), depth_shader_set_up, DEPTH_NAME);
	model_ptr->addTexture(TEXTURE_NAME, "textures/monkey.DDS");
	model_ptr->setLightName(LIGHT_NAME);
	model_ptr->addInstance("Monkey", SingleModelInstance(shared_ptr<Base>{new Base()}, STANDARD_SHADER_NAME, TEXTURE_NAME, MATERIAL_NAME, standard_set_up_shader));

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
	vertex_texture_model_ptr->addTexture("Wood_Texture", model_ptr->getBufferTexture());
	vertex_texture_model_ptr->addInstance("Overview", VertexTextureInstance(shared_ptr<Base> {new Base()}, WOOD_NAME, "Wood_Texture", wood_shader_set_up));
	world->addModel("Overview", vertex_texture_model_ptr);

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		manager.getCurrentController().control(delta);

		{
			glBindFramebuffer(GL_FRAMEBUFFER, model_ptr->getFrameBuffer());
			vector<GLint> border(4);
			glGetIntegerv(GL_VIEWPORT, &border[0]);
			glViewport(0, 0, model_ptr->getSize()[0], model_ptr->getSize()[1]);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			{
				model_ptr->draw();
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(border[0], border[1], border[2], border[3]);
		}

		world->update_draw(delta);
	}
}