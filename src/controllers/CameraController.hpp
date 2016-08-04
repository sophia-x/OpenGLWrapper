#ifndef CAMERA_CONTROLLER
#define CAMERA_CONTROLLER

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
using namespace glm;

class CameraController {
public:
	CameraController(GLFWwindow *p_window, vec3 p_position, float p_horizontal_angle, float p_vertical_angle, float p_fov):
		window{p_window}, position{p_position}, horizontal_angle{p_horizontal_angle}, vertical_angle(p_vertical_angle), fov(p_fov) {}
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
	float horizontal_angle;
	float vertical_angle;
	float fov;
};

#endif