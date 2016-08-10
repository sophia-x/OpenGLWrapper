#include <sstream>

#include <gui/WindowManager.hpp>

#include <controllers/RotateCameraController.hpp>

#include <model/World.hpp>
#include <model/Light.hpp>
#include <model/SingleModel.hpp>
#include <model/Material.hpp>

#include <tools/Timer.hpp>

#include <AntTweakBar.h>

#include "Demo.hpp"

#define TITLE "Pick by Color"
#define LIGHT_NAME "Light"
#define MATERIAL_NAME "Material"
#define TEXTURE_NAME "TEXTURE"

static void pick_by_color(SingleModel &model, string name);
static void init_pick_up_shader(World *world, const string &vertex_path, const string &freg_path, const string &name);

static string message;
static void setTwUI() {
	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WIDTH, HEIGHT);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &message, NULL);
}

void pick_by_color_demo() {
	WindowManager &manager = WindowManager::getWindowManager();

	WindowManager::getWindowManager().addWindow(NAME, createWindow(WIDTH, HEIGHT, TITLE, NAME));
	manager.setCurrent(NAME);
	manager.turnGlewOn();
	manager.addCurrentController(new RotateCameraController(manager.getCurrentWindow(), vec3(0, 0, 5), 45.0f, 1.0f, 0.005f));
	setTwUI();
	opengl_init(BG_COLOR);

	World *world = new World();
	manager.addWorld(NAME, world);
	manager.setCurrentWorld(NAME);

	static const string STANDARD_SHADER_NAME  = "Standard_Shader";
	init_standard_shader(world, "shaders/StandardShading.vertexshader", "shaders/StandardShading.fragmentshader", STANDARD_SHADER_NAME);
	static const string PICK_UP_SHADER_NAME = "Pick_Up_Shader";
	init_pick_up_shader(world, "shaders/Picking.vertexshader", "shaders/Picking.fragmentshader", PICK_UP_SHADER_NAME);


	PointLight *light = new PointLight(vec3(0), vec3(1), 50.f);
	world->addLight(LIGHT_NAME, light);

	PhoneMaterial *material = new PhoneMaterial(vec3(0.1f), vec3(0.3f), 5);
	world->addMaterial(MATERIAL_NAME, material);

	SingleModel *model_ptr = new SingleModel("models/monkey.obj");
	model_ptr->addTexture(TEXTURE_NAME, "textures/monkey.DDS");
	model_ptr->setLightName(LIGHT_NAME);
	for (int i = 0; i < 100; i++) {
		vec3 pos = vec3(rand() % 20 - 10, rand() % 20 - 10, rand() % 20 - 10);
		quat orientation = quat(vec3(rand() % 360, rand() % 360, rand() % 360));
		model_ptr->addInstance(to_string(i), SingleModelInstance(STANDARD_SHADER_NAME, TEXTURE_NAME, MATERIAL_NAME,
		                       standard_set_up_shader, pos, orientation));
	}
	world->addModel("Monkey", model_ptr);

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		manager.getCurrentController().control(delta);

		if (glfwGetMouseButton(WindowManager::getWindowManager().getCurrentWindow(), GLFW_MOUSE_BUTTON_LEFT)) {
			pick_by_color(*model_ptr, PICK_UP_SHADER_NAME);
			glfwSwapBuffers(WindowManager::getWindowManager().getCurrentWindow());
			continue;
		}

		world->update_draw(delta);

		// Draw GUI
		TwDraw();
	}

	TwTerminate();
}

void pick_by_color(SingleModel &model, string name) {
	// Clear the screen in white
	static GLfloat color[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model.useVertexBuffer();

	map<string, SingleModelInstance>& instances = model.getInstances();

	for (auto it = instances.begin(); it != instances.end(); it++) {
		const Shader &shader = WindowManager::getWindowManager().getCurrentWorld().getShader(name);
		glUseProgram(shader.getID());

		SingleModelInstance& ins = it->second;

		const mat4 &model_matrix = ins.getModelMatrix();
		const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
		const mat4 &projection_matrix = controller.getProjectionMatrix();
		const mat4 &view_matrix = controller.getViewMatrix();
		mat4 MVP = projection_matrix * view_matrix * model_matrix;
		glUniformMatrix4fv(shader.getUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

		int i = stoi(it->first);
		int r = (i & 0x000000FF) >>  0;
		int g = (i & 0x0000FF00) >>  8;
		int b = (i & 0x00FF0000) >> 16;
		glUniform4f(shader.getUniform("picking_color"), r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);

		model.draw_mesh();
	}

	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned char data[4];
	glReadPixels(WIDTH / 2, HEIGHT / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

	int pickedID =
	    data[0] +
	    data[1] * 256 +
	    data[2] * 256 * 256;

	if (pickedID == 0x00ffffff) {
		message = "background";
	} else {
		std::ostringstream oss;
		oss << "mesh " << pickedID;
		message = oss.str();
	}

	glClearColor(color[0], color[1], color[2], color[3]);
}

void init_pick_up_shader(World *world, const string &vertex_path, const string &freg_path, const string &name) {
	vector<string> names {
		"MVP",
		"picking_color"
	};
	world->addShader(name, new Shader(vertex_path, freg_path, names));
}