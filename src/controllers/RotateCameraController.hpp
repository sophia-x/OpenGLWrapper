#ifndef ROTATE_CAMERA_CONTROLLER
#define ROTATE_CAMERA_CONTROLLER

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

#include "CameraController.hpp"

class RotateCameraController: public CameraController {
public:
	RotateCameraController(GLFWwindow *window, vec3 p_position, float p_fov, float p_speed, float p_mouse_speed,
	                       vec3 p_up = vec3(0, 1, 0), vec3 p_origin = vec3(0), float p_ratio = 4.0f / 3.0f, float p_near = 0.1f,
	                       float p_far = 100.f): CameraController{window, p_position, p_fov, p_ratio, p_near, p_far}, speed{p_speed},
		mouse_speed{p_mouse_speed}, up{normalize(p_up)}, origin{p_origin}, x_angle{0}, y_angle{0}, p_ypos{0},
		dis{l2Norm(p_position - p_origin)}
	{
		// Hide the mouse and enable unlimited mouvement
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}


	void control(float delta) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		dis += mouse_speed * (ypos - p_ypos);
		p_ypos = ypos;

		if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS) {
			x_angle += delta * speed;
		}
		if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS) {
			x_angle -= delta * speed;
		}
		if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS) {
			y_angle += delta * speed;
		}
		if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS) {
			y_angle -= delta * speed;
		}

		vec3 dir = position - origin;
		vec3 y_dir = rotate(dir, y_angle, up);
		vec3 right = normalize(cross(up, y_dir));
		vec3 xy_dir = rotate(y_dir, x_angle, right);
		xy_dir = xy_dir / l2Norm(xy_dir) * dis;
		vec3 xy_up = normalize(cross(xy_dir, right));
		vec3 xy_position = xy_dir + origin;

		projection_matrix = glm::perspective(glm::radians(fov), ratio, near, far);
		// Camera matrix
		view_matrix       = glm::lookAt(
		                        xy_position,
		                        origin,
		                        xy_up
		                    );
	}

private:
	float speed, mouse_speed;
	float x_angle, y_angle;
	float p_ypos;
	float dis;

	vec3 up, origin;
};

#endif