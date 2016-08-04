#include <glm/gtx/transform.hpp>

#include "MouseCameraController.hpp"

void MouseCameraController::control(float delta) {
	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Compute new orientation
	horizontal_angle += mouse_speed * float(p_xpos - xpos );
	vertical_angle    += mouse_speed * float(p_ypos - ypos );
	p_xpos = xpos; p_ypos = ypos;

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	vec3 direction(
	    cos(vertical_angle) * sin(horizontal_angle),
	    sin(vertical_angle),
	    cos(vertical_angle) * cos(horizontal_angle)
	);

	// Right vector
	vec3 right = glm::vec3(
	                 sin(horizontal_angle - 3.14f / 2.0f),
	                 0,
	                 cos(horizontal_angle - 3.14f / 2.0f)
	             );

	// Up vector
	vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS) {
		position += direction * delta * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS) {
		position -= direction * delta * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS) {
		position += right * delta * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS) {
		position -= right * delta * speed;
	}

	if (scrolled[(unsigned long)this] && scroll_window == window) {
		fov -= 0.3 * scroll_y;
		scrolled[(unsigned long)this] = false;
	}

	projection_matrix = glm::perspective(glm::radians(fov), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	view_matrix       = glm::lookAt(
	                        position,
	                        position + direction,
	                        up
	                    );
}