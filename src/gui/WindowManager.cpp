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

void WindowManager::turnDebugOn(void APIENTRY(*p_DebugOutputCallback)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*)) {
	if ( GLEW_ARB_debug_output ) {
		printf("OpengL Debug turned on.\n");
		glDebugMessageCallbackARB(p_DebugOutputCallback, NULL);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	} else {
		printf("ARB_debug_output unavailable.\n");
	}
}

void APIENTRY DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void* userParam) {
	fprintf(stderr, "OpenGL Debug Output message:\n");

	if (source == GL_DEBUG_SOURCE_API_ARB)
		fprintf(stderr, "Source : API; ");
	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
		fprintf(stderr, "Source : WINDOW_SYSTEM; ");
	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
		fprintf(stderr, "Source : SHADER_COMPILER; ");
	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
		fprintf(stderr, "Source : THIRD_PARTY; ");
	else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)
		fprintf(stderr, "Source : APPLICATION; ");
	else if (source == GL_DEBUG_SOURCE_OTHER_ARB)
		fprintf(stderr, "Source : OTHER; ");

	fprintf(stderr, "\n");

	if (type == GL_DEBUG_TYPE_ERROR_ARB)
		fprintf(stderr, "Type : ERROR; ");
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)
		fprintf(stderr, "Type : DEPRECATED_BEHAVIOR; ");
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)
		fprintf(stderr, "Type : UNDEFINED_BEHAVIOR; ");
	else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)
		fprintf(stderr, "Type : PORTABILITY; ");
	else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)
		fprintf(stderr, "Type : PERFORMANCE; ");
	else if (type == GL_DEBUG_TYPE_OTHER_ARB)
		fprintf(stderr, "Type : OTHER; ");

	fprintf(stderr, "\n");

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)
		fprintf(stderr, "Severity : HIGH; ");
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
		fprintf(stderr, "Severity : MEDIUM; ");
	else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)
		fprintf(stderr, "Severity : LOW; ");

	fprintf(stderr, "\n");

	fprintf(stderr, "Message: %s\n", message);
}

GLFWwindow* createWindow(unsigned int width, unsigned int height, const string &title, const string &name) {
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

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

	glEnable(GL_CULL_FACE);
}