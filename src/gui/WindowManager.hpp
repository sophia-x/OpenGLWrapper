#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <string>
#include <map>
#include <stack>
#include <memory>
#include <mutex>
using namespace std;

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
using namespace glm;

#include <controllers/CameraController.hpp>
#include <model/World.hpp>

class WindowManager {
public:
	static WindowManager& getWindowManager();

	~WindowManager() {
		// Close OpenGL window and terminate GLFW
		glfwTerminate();
	}

	// Must called after setCurrent
	void turnGlewOn();

	inline void addWindow(const string &name, GLFWwindow* window) {
		windows[name] = window;
	}

	inline void addCurrentController(CameraController *controller) {
		controllers[currentName].push(unique_ptr<CameraController>(controller));
	}

	inline void addWorld(const string &name, World *world) {
		worlds[name] = unique_ptr<World>(world);
	}

	inline void setCurrent(const string &name) {
		currentName = name;
		currentWindow = windows[currentName];

		glfwMakeContextCurrent(currentWindow);
	}

	inline void setCurrentWorld(const string &name) {
		current_world_name = name;
	}

	inline GLFWwindow* getWindow(const string &name) {
		return windows[name];
	}

	inline GLFWwindow* getCurrentWindow() {
		return currentWindow;
	}

	inline CameraController& getCurrentController() {
		return *controllers[currentName].top();
	}

	inline World& getCurrentWorld() {
		return *worlds[current_world_name];
	}

	inline bool next() const {
		// Swap buffers
		glfwSwapBuffers(currentWindow);
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();

		return glfwGetKey(currentWindow, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(currentWindow) == 0;
	}

private:
	static unique_ptr<WindowManager> manager_ptr;
	static once_flag created;
	WindowManager();
	WindowManager(const WindowManager& src);
	WindowManager& operator=(const WindowManager& rhs);

private:
	map<string, GLFWwindow*> windows;
	map<string, stack<unique_ptr<CameraController> > > controllers;
	map<string, unique_ptr<World> > worlds;

	string currentName;
	GLFWwindow* currentWindow;
	string current_world_name;
};

GLFWwindow* createWindow(unsigned int width, unsigned int height, const string &title, const string &name);

void opengl_init(vec4 bg_color);

#endif