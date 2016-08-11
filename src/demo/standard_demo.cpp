#include <sstream>

#include <gui/WindowManager.hpp>

#include <controllers/RotateCameraController.hpp>

#include <model/World.hpp>
#include <model/Light.hpp>
#include <model/SingleNormalMapModel.hpp>
#include <model/DebugModel.hpp>
#include <model/Material.hpp>
#include <model/TextModel.hpp>

#include <tools/Timer.hpp>

#include "Demo.hpp"

#define TITLE "Standard Demo"
#define LIGHT_NAME "Light"
#define MATERIAL_NAME "Material"
#define TEXTURE_NAME "TEXTURE"

void standard_demo() {
	WindowManager &manager = WindowManager::getWindowManager();

	WindowManager::getWindowManager().addWindow(NAME, createWindow(WIDTH, HEIGHT, TITLE, NAME));
	manager.setCurrent(NAME);
	manager.turnGlewOn();
	manager.turnDebugOn(DebugOutputCallback);
	manager.addCurrentController(new RotateCameraController(manager.getCurrentWindow(), vec3(0, 0, 5), 45.0f, 1.0f, 0.005f));
	opengl_init(BG_COLOR);

	World *world = new World();
	manager.addWorld(NAME, world);
	manager.setCurrentWorld(NAME);

	shared_ptr<Base> base_ptr{new Base()};

	static const string STANDARD_SHADER_NAME  = "Standard_Shader";
	init_standard_shader(world, "shaders/StandardShading.vertexshader", "shaders/StandardShading.fragmentshader", STANDARD_SHADER_NAME);

	PointLight *light = new PointLight(vec3(4), vec3(1), 50.f);
	world->addLight(LIGHT_NAME, light);

	PhoneMaterial *material = new PhoneMaterial(vec3(0.1f), vec3(0.3f), 5);
	world->addMaterial(MATERIAL_NAME, material);

	// SingleNormalMapModel *model_ptr = new SingleNormalMapModel("models/monkey.obj");
	SingleNormalMapModel *model_ptr = new SingleNormalMapModel("models/cylinder.obj");
	model_ptr->addTexture(TEXTURE_NAME, "textures/monkey.DDS");
	model_ptr->setLightName(LIGHT_NAME);
	model_ptr->addInstance("Monkey", SingleModelInstance(base_ptr, STANDARD_SHADER_NAME, TEXTURE_NAME, MATERIAL_NAME, standard_set_up_shader));
	world->addModel("Monkey", model_ptr);

	static const string TEXT_SHADER_NAME  = "Text_Shader";
	init_text_shader(world, "shaders/TextVertexShader.vertexshader", "shaders/TextVertexShader.fragmentshader", TEXT_SHADER_NAME);
	TextModel *text_model_ptr = new TextModel(800, 600, "textures/Holstein.DDS", TEXT_SHADER_NAME, text_set_up_shader);
	world->addModel("Text", text_model_ptr);

	static const string DEBUG_SHADER_NAME = "Debug_Shader";
	init_debug_shader(world, "shaders/DebugVertexShader.vertexshader", "shaders/DebugFragmentShader.fragmentshader", DEBUG_SHADER_NAME);
	DebugModel *normal_debug_model_ptr = new DebugModel(vec4(0, 0, 1, 1), GL_LINES);
	normal_debug_model_ptr->addInstance("Monkey", Instance(base_ptr, DEBUG_SHADER_NAME, debug_set_up_shader));
	DebugModel *tangent_debug_model_ptr = new DebugModel(vec4(1, 0, 0, 1), GL_LINES);
	tangent_debug_model_ptr->addInstance("Monkey", Instance(base_ptr, DEBUG_SHADER_NAME, debug_set_up_shader));
	DebugModel *bitagent_debug_model_ptr = new DebugModel(vec4(0, 1, 0, 1), GL_LINES);
	bitagent_debug_model_ptr->addInstance("Monkey", Instance(base_ptr, DEBUG_SHADER_NAME, debug_set_up_shader));
	world->addModel("Normal", normal_debug_model_ptr);
	world->addModel("Tangent", tangent_debug_model_ptr);
	world->addModel("Bitagent", bitagent_debug_model_ptr);

	{
		const vector<unsigned short>& indices = model_ptr->getIndices();
		const vector<vec3>& vertices = model_ptr->getVertices();
		const vector<vec2>& uvs = model_ptr->getUVs();
		const vector<vec3>& normals = model_ptr->getNormals();

		const vector<vec3>& indexed_tangents = model_ptr->getTangents();
		const vector<vec3>& indexed_bitagents = model_ptr->getBitangents();

		for (unsigned int i = 0; i < vertices.size(); i++) {
			const vec3& p = vertices[i];
			normal_debug_model_ptr->addVertex(p);
			tangent_debug_model_ptr-> addVertex(p);
			bitagent_debug_model_ptr->addVertex(p);

			vec3 n = p + 0.1f * normalize(normals[i]);
			normal_debug_model_ptr->addVertex(n);

			vec3 t = p + 0.1f * normalize(indexed_tangents[i]);
			tangent_debug_model_ptr->addVertex(t);

			vec3 b = p + 0.1f * normalize(indexed_bitagents[i]);
			bitagent_debug_model_ptr->addVertex(b);
		}
	}

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();
		ostringstream oss;
		oss << timer.getFrameRate() << " frames/s";
		text_model_ptr->clear();
		text_model_ptr->print_text(oss.str(), 10, 500, 60);

		manager.getCurrentController().control(delta);

		world->update_draw(delta);
	}
}