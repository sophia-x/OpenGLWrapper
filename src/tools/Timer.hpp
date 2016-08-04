#ifndef TIMER
#define TIMER

#include <GLFW/glfw3.h>

class Timer {
public:
	Timer(): p_time{glfwGetTime()} {}

	inline void tick() {
		double current_time = glfwGetTime();
		delta = current_time - p_time;
		p_time = current_time;
	}

	inline double getDelta() const {
		return delta;
	}

private:
	double p_time;
	double delta;
};

#endif