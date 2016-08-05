#ifndef MOUSE_CAMERA_CONTROLLER
#define MOUSE_CAMERA_CONTROLLER

#include <functional>
using namespace std;

#include <tools/CallBackHelper.hpp>
#include "CameraController.hpp"


class MouseCameraController: public CameraController {
public:
	MouseCameraController(GLFWwindow *window, vec3 p_position, float p_fov, float p_horizontal_angle, float p_vertical_angle,
	                      float p_speed, float p_mouse_spped, float p_fov_speed, float p_ratio = 4.0f / 3.0f, float p_near = 0.1f, float p_far = 100.f):
		CameraController{window, p_position, p_fov, p_ratio, p_near, p_far}, horizontal_angle{p_horizontal_angle},
		vertical_angle{p_vertical_angle}, p_xpos{0}, p_ypos{0}, speed{p_speed}, mouse_speed{p_mouse_spped}, fov_speed{p_fov_speed}
	{
		// Hide the mouse and enable unlimited mouvement
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetScrollCallback(window, scrollCallBack);
		scrolled[(unsigned long)this] = false;
	}

	void control(float delta);

private:
	float horizontal_angle;
	float vertical_angle;
	float p_xpos, p_ypos;
	float speed;
	float mouse_speed;
	float fov_speed;
};

#endif