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

#define TITLE "Pick by UDF"
#define LIGHT_NAME "Light"
#define MATERIAL_NAME "Material"
#define TEXTURE_NAME "TEXTURE"

static void ScreenPosToWorldRay(
    int mouseX, int mouseY, int screenWidth, int screenHeight, mat4 ViewMatrix, mat4 ProjectionMatrix,
    vec3& out_origin, vec3& out_direction
);

static bool TestRayOBBIntersection(
    vec3 ray_origin,        // Ray origin, in world space
    vec3 ray_direction,     // Ray direction, in world space.
    vec3 aabb_min,          // Minimum X,Y,Z coords of the mes.
    vec3 aabb_max,          // Maximum X,Y,Z coords.
    mat4 ModelMatrix,       // Transformation applied to the mesh
    float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
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

void pick_by_udf_demo() {
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

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		manager.getCurrentController().control(delta);

		if (glfwGetMouseButton(WindowManager::getWindowManager().getCurrentWindow(), GLFW_MOUSE_BUTTON_LEFT)) {
			vec3 ray_origin;
			vec3 ray_direction;
			const CameraController& controller = WindowManager::getWindowManager().getCurrentController();
			const mat4 &projection_matrix = controller.getProjectionMatrix();
			const mat4 &view_matrix = controller.getViewMatrix();
			ScreenPosToWorldRay(
			    WIDTH / 2, HEIGHT / 2,
			    WIDTH, HEIGHT,
			    view_matrix,
			    projection_matrix,
			    ray_origin,
			    ray_direction
			);

			message = "background";
			float min_dist = -1;
			map<string, SingleModelInstance>& instances = model_ptr->getInstances();
			for (auto it = instances.begin(); it != instances.end(); it++) {
				SingleModelInstance& ins = it->second;

				float intersection_distance;
				vec3 aabb_min(-1.0f, -1.0f, -1.0f);
				vec3 aabb_max( 1.0f,  1.0f,  1.0f);

				if ( TestRayOBBIntersection(
				            ray_origin,
				            ray_direction,
				            aabb_min,
				            aabb_max,
				            ins.getModelMatrix(),
				            intersection_distance)
				   ) {
					if (min_dist < 0 || intersection_distance < min_dist) {
						min_dist = intersection_distance;
						std::ostringstream oss;
						oss << "mesh " << it->first;
						message = oss.str();
						break;
					}
				}
			}
		}

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

bool TestRayOBBIntersection(
    vec3 ray_origin,
    vec3 ray_direction,
    vec3 aabb_min,
    vec3 aabb_max,
    mat4 ModelMatrix,
    float& intersection_distance
) {
	float tMin = 0.0f;
	float tMax = 100000.0f;

	vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

	vec3 delta = OBBposition_worldspace - ray_origin;
	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
		float e = dot(xaxis, delta);
		float f = dot(ray_direction, xaxis);

		if ( fabs(f) > 0.001f ) { // Standard case
			float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
			float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
			if (t1 > t2) {
				float w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if ( t2 < tMax )
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if ( t1 > tMin )
				tMin = t1;
			if (tMax < tMin )
				return false;
		} else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
				return false;
		}
	}

	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	{
		vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
		float e = dot(yaxis, delta);
		float f = dot(ray_direction, yaxis);

		if ( fabs(f) > 0.001f ) {

			float t1 = (e + aabb_min.y) / f;
			float t2 = (e + aabb_max.y) / f;

			if (t1 > t2) {float w = t1; t1 = t2; t2 = w;}

			if ( t2 < tMax )
				tMax = t2;
			if ( t1 > tMin )
				tMin = t1;
			if (tMin > tMax)
				return false;

		} else {
			if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
				return false;
		}
	}

	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	{
		vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
		float e = dot(zaxis, delta);
		float f = dot(ray_direction, zaxis);

		if ( fabs(f) > 0.001f ) {

			float t1 = (e + aabb_min.z) / f;
			float t2 = (e + aabb_max.z) / f;

			if (t1 > t2) {float w = t1; t1 = t2; t2 = w;}

			if ( t2 < tMax )
				tMax = t2;
			if ( t1 > tMin )
				tMin = t1;
			if (tMin > tMax)
				return false;

		} else {
			if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
				return false;
		}
	}

	intersection_distance = tMin;
	return true;
}