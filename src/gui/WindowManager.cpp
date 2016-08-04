#include <stdio.h>
#include <stdlib.h>

#include "WindowManager.hpp"

unique_ptr<WindowManager> WindowManager::manager_ptr;
once_flag WindowManager::created;

WindowManager& WindowManager::getWindowManager() {
	call_once(created,
	[] {
		manager_ptr.reset(new WindowManager);
	});
	return *manager_ptr.get();
}

WindowManager::WindowManager() {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}
}

void WindowManager::turnGlewOn() {
	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
}

GLFWwindow* createWindow(unsigned int width, unsigned int height, const string &title, const string &name) {
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	return window;
}

void opengl_init(vec4 bg_color) {
	glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
}