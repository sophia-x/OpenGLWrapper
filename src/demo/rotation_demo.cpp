#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include <gui/WindowManager.hpp>

#include <controllers/CameraController.hpp>

#include <model/World.hpp>
#include <model/Light.hpp>
#include <model/SingleModel.hpp>
#include <model/Material.hpp>

#include <tools/Timer.hpp>
#include <tools/quaternion.hpp>

#include <AntTweakBar.h>

#include "Demo.hpp"

#define TITLE "Rotation"
#define QUATERN_LOC "808 16"
#define LIGHT_NAME "Light"
#define MATERIAL_NAME "Material"
#define TEXTURE_NAME "TEXTURE"
#define EULER "Euler"
#define QUAT "Quat"

vec3 euler_pos(-1.5f, 0.0f, 0.0f);
vec3 euler_orig;

vec3 quat_pos( 1.5f, 0.0f, 0.0f);
quat quat_orig;

bool face_to = true;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	TwEventMouseButtonGLFW(button, action);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	TwEventMousePosGLFW(xpos, ypos);
}

static void setTwUI() {
	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WIDTH, HEIGHT);
	TwBar *EulerGUI = TwNewBar("Euler settings");
	TwBar *QuaternionGUI = TwNewBar("Quaternion settings");
	TwSetParam(EulerGUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwSetParam(QuaternionGUI, NULL, "position", TW_PARAM_CSTRING, 1, QUATERN_LOC);

	TwAddVarRW(EulerGUI, "Euler X", TW_TYPE_FLOAT, &euler_orig.x, "step=0.01");
	TwAddVarRW(EulerGUI, "Euler Y", TW_TYPE_FLOAT, &euler_orig.y, "step=0.01");
	TwAddVarRW(EulerGUI, "Euler Z", TW_TYPE_FLOAT, &euler_orig.z, "step=0.01");
	TwAddVarRW(EulerGUI, "Pos X"  , TW_TYPE_FLOAT, &euler_pos.x, "step=0.1");
	TwAddVarRW(EulerGUI, "Pos Y"  , TW_TYPE_FLOAT, &euler_pos.y, "step=0.1");
	TwAddVarRW(EulerGUI, "Pos Z"  , TW_TYPE_FLOAT, &euler_pos.z, "step=0.1");

	TwAddVarRW(QuaternionGUI, "Quaternion", TW_TYPE_QUAT4F, &quat_orig, "showval=true  opened=true");
	TwAddVarRW(QuaternionGUI, "Face to", TW_TYPE_BOOL8 , &face_to, "help='Face to the other monkey ?'");

	glfwSetMouseButtonCallback(WindowManager::getWindowManager().getCurrentWindow(), mouse_button_callback);
	glfwSetCursorPosCallback(WindowManager::getWindowManager().getCurrentWindow(), cursor_position_callback);
}

void rotation_demo() {
	WindowManager &manager = WindowManager::getWindowManager();

	WindowManager::getWindowManager().addWindow(NAME, createWindow(WIDTH, HEIGHT, TITLE, NAME));
	manager.setCurrent(NAME);
	manager.turnGlewOn();
	manager.addCurrentController(new StaticCameraController(manager.getCurrentWindow(), vec3(0, 0, 7)));
	setTwUI();
	opengl_init(BG_COLOR);

	World *world = new World();
	manager.addWorld(NAME, world);
	manager.setCurrentWorld(NAME);

	static const string STANDARD_SHADER_NAME  = "Standard_Shader";
	init_standard_shader(world, "shaders/StandardShading.vertexshader", "shaders/StandardShading.fragmentshader", STANDARD_SHADER_NAME);

	PointLight *light = new PointLight(vec3(4), vec3(1), 50.f);
	world->addLight(LIGHT_NAME, light);

	PhoneMaterial *material = new PhoneMaterial(vec3(0.1f), vec3(0.3f), 5);
	world->addMaterial(MATERIAL_NAME, material);

	SingleModel *model_ptr = new SingleModel("models/monkey.obj");
	model_ptr->addTexture(TEXTURE_NAME, "textures/monkey.DDS");
	model_ptr->setLightName(LIGHT_NAME);
	model_ptr->addInstance(EULER, SingleModelInstance(STANDARD_SHADER_NAME, TEXTURE_NAME, MATERIAL_NAME, standard_set_up_shader));
	model_ptr->addInstance(QUAT, SingleModelInstance(STANDARD_SHADER_NAME, TEXTURE_NAME, MATERIAL_NAME, standard_set_up_shader));

	world->addModel("Monkey", model_ptr);

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		manager.getCurrentController().control(delta);

		{
			model_ptr->setPos(EULER, euler_pos);
			model_ptr->setPos(QUAT, quat_pos);

			euler_orig.y += 3.14159f / 2.0f * delta;
			model_ptr->setOrientation(EULER, toQuat(eulerAngleYXZ(euler_orig.y, euler_orig.x, euler_orig.z)));

			if (face_to) {
				vec3 dir = euler_pos - quat_pos;
				vec3 up = vec3(0.0f, 1.0f, 0.0f);
				// Compute the desired orientation
				quat orig = normalize(lookAt(dir, up));
				// And interpolate
				quat_orig = rotateTowards(quat_orig, orig, 1.0f * delta);
			}
			model_ptr->setOrientation(QUAT, quat_orig);
		}

		world->update_draw(delta);

		// Draw GUI
		TwDraw();
	}

	TwTerminate();
}