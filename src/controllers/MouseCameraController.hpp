#ifndef MOUSE_CAMERA_CONTROLLER
#define MOUSE_CAMERA_CONTROLLER

#include <functional>
using namespace std;

#include <tools/CallBackHelper.hpp>
#include "CameraController.hpp"


class MouseCameraController: public CameraController {
public:
	MouseCameraController(GLFWwindow *window, vec3 p_position, float p_horizontal_angle, float p_vertical_angle, float p_fov,
	                      float p_speed, float p_mouse_spped): CameraController{window, p_position, p_horizontal_angle,
		                              p_vertical_angle, p_fov}, p_xpos{0}, p_ypos{0}, speed(p_speed), mouse_speed(p_mouse_spped)
	{
		// Hide the mouse and enable unlimited mouvement
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetScrollCallback(window, scrollCallBack);
		scrolled[(unsigned long)this] = false;
	}

	void control(float delta);

private:
	float p_xpos, p_ypos;
	float speed; // 3 units / second
	float mouse_speed;
};

#endif