#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

class CameraController {
public:
	CameraController(GLFWwindow *p_window, vec3 p_position, float p_fov, float p_ratio, float p_near, float p_far):
		window{p_window}, position{p_position}, fov(p_fov), ratio{p_ratio}, near{p_near}, far{p_far} {}
	virtual ~CameraController() {}

	virtual void control(float delta) = 0;

	const mat4& getViewMatrix() const {
		return view_matrix;
	};
	const mat4& getProjectionMatrix() const {
		return projection_matrix;
	};
	const vec3& getCameraPosition() const {
		return position;
	};

protected:
	GLFWwindow *window;

	mat4 view_matrix;
	mat4 projection_matrix;
	vec3 position;

	float fov;
	float ratio, near, far;
};

class StaticCameraController: public CameraController {
public:
	StaticCameraController(GLFWwindow *p_window, vec3 p_position, float p_fov = 45.0f, float p_ratio = 4.0f / 3.0f,
	                       float p_near = 0.1f, float p_far = 100.f, vec3 p_direction = vec3(0, 0, 0), vec3 p_up = vec3(0, 1, 0)):
		CameraController{p_window, p_position, p_fov, p_ratio, p_near, p_far}
	{
		projection_matrix = perspective(radians(fov), ratio, near, far);
		view_matrix = lookAt(position, p_direction, p_up);
	}

	inline void control(float delta) {}
};

#endif