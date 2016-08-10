#include <sstream>

#include <gui/WindowManager.hpp>

#include <controllers/RotateCameraController.hpp>

#include <model/World.hpp>
#include <model/Light.hpp>
#include <model/SingleModel.hpp>
#include <model/Material.hpp>

#include <tools/Timer.hpp>

#include <AntTweakBar.h>

#include <btBulletDynamicsCommon.h>

#include "Demo.hpp"

#define TITLE "Pick by Bullet"
#define LIGHT_NAME "Light"
#define MATERIAL_NAME "Material"
#define TEXTURE_NAME "TEXTURE"

class BulletDebugDrawer_OpenGL : public btIDebugDraw {
public:
	BulletDebugDrawer_OpenGL(): shader{"shaders/DebugVertexShader.vertexshader", "shaders/DebugFragmentShader.fragmentshader",
		                                   vector<string>{"COLOR", "MVP"}} {
		glGenVertexArrays(1, &DebugArrayID);
		glBindVertexArray(DebugArrayID);

		glGenBuffers(1, &debugbuffer);
		glBindVertexArray(0);
	}

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
		vec3 line[2];
		line[0] = vec3(from.x(), from.y(), from.z());
		line[1] = vec3(to.x(), to.y(), to.z());

		glBindVertexArray(DebugArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, debugbuffer);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), &line[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );

		glUseProgram(shader.getID());
		glUniform3f(shader.getUniform("COLOR"), color.x(), color.y(), color.z());
		const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
		const mat4 &projection_matrix = controller.getProjectionMatrix();
		const mat4 &view_matrix = controller.getViewMatrix();
		mat4 MVP = projection_matrix * view_matrix;
		glUniformMatrix4fv(shader.getUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

		glDrawArrays(GL_LINES, 0, 2);

		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
	virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) {}
	virtual void reportErrorWarning(const char *) {}
	virtual void draw3dText(const btVector3 &, const char *) {}
	virtual void setDebugMode(int p) {
		m = p;
	}
	int getDebugMode(void) const {return 3;}
	int m;

	GLuint DebugArrayID, debugbuffer;
	Shader shader;
};

static void ScreenPosToWorldRay(
    int mouseX, int mouseY, int screenWidth, int screenHeight, mat4 ViewMatrix, mat4 ProjectionMatrix,
    vec3& out_origin, vec3& out_direction
);

static string message;
static void setTwUI() {
	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WIDTH, HEIGHT);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &message, NULL);
}

void pick_by_bullet_demo() {
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


	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
	btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));

	BulletDebugDrawer_OpenGL mydebugdrawer;
	dynamicsWorld->setDebugDrawer(&mydebugdrawer);

	map<string, SingleModelInstance>& instances = model_ptr->getInstances();
	for (auto it = instances.begin(); it != instances.end(); it++) {
		SingleModelInstance& ins = it->second;


		btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
		            btQuaternion(ins.origentation.x, ins.origentation.y, ins.origentation.z, ins.origentation.w),
		            btVector3(ins.pos.x, ins.pos.y, ins.pos.z)
		        ));
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
		    0,                  // mass, in kg. 0 -> Static object, will never move.
		    motionstate,
		    boxCollisionShape,  // collision shape of body
		    btVector3(0, 0, 0)  // local inertia
		);
		btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);
		dynamicsWorld->addRigidBody(rigidBody);
		rigidBody->setUserPointer((void*)it->first.c_str());
	}

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		manager.getCurrentController().control(delta);

		dynamicsWorld->stepSimulation(delta, 7);

		if (glfwGetMouseButton(WindowManager::getWindowManager().getCurrentWindow(), GLFW_MOUSE_BUTTON_LEFT)) {
			vec3 out_origin;
			vec3 out_direction;
			const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
			const mat4 &projection_matrix = controller.getProjectionMatrix();
			const mat4 &view_matrix = controller.getViewMatrix();
			ScreenPosToWorldRay(
			    WIDTH / 2, HEIGHT / 2,
			    WIDTH, HEIGHT,
			    view_matrix,
			    projection_matrix,
			    out_origin,
			    out_direction
			);
			vec3 out_end = out_origin + out_direction * 1000.0f;

			btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(out_origin.x, out_origin.y, out_origin.z),
			        btVector3(out_end.x, out_end.y, out_end.z));
			dynamicsWorld->rayTest(btVector3(out_origin.x, out_origin.y, out_origin.z),
			                       btVector3(out_end.x, out_end.y, out_end.z), RayCallback);
			if (RayCallback.hasHit()) {
				std::ostringstream oss;
				oss << "mesh " << (const char*)RayCallback.m_collisionObject->getUserPointer();
				message = oss.str();
			} else {
				message = "background";
			}
		}

		dynamicsWorld->debugDrawWorld();

		world->update_draw(delta);

		// Draw GUI
		TwDraw();
	}

	TwTerminate();
}

void ScreenPosToWorldRay(
    int mouseX, int mouseY, int screenWidth, int screenHeight, mat4 ViewMatrix, mat4 ProjectionMatrix,
    vec3& out_origin, vec3& out_direction
) {
	vec4 lRayStart_NDC(
	    ((float)mouseX / (float)screenWidth  - 0.5f) * 2.0f,
	    ((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
	    -1.0,
	    1.0f
	);
	vec4 lRayEnd_NDC(
	    ((float)mouseX / (float)screenWidth  - 0.5f) * 2.0f,
	    ((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
	    0.0,
	    1.0f
	);
	mat4 InverseProjectionMatrix = inverse(ProjectionMatrix);
	mat4 InverseViewMatrix = inverse(ViewMatrix);

	mat4 M = inverse(ProjectionMatrix * ViewMatrix);
	vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world /= lRayStart_world.w;
	vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /= lRayEnd_world.w;

	vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = normalize(lRayDir_world);

	out_origin = vec3(lRayStart_world);
	out_direction = normalize(lRayDir_world);
}