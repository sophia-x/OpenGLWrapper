#ifndef TIMER
#define TIMER

#include <stdio.h>

#include <GLFW/glfw3.h>

class Timer {
public:
	Timer(): p_time{glfwGetTime()}, passed{0}, n_frames{0} {}

	inline void tick() {
		double current_time = glfwGetTime();
		delta = current_time - p_time;
		p_time = current_time;

		passed += delta;
		n_frames ++;
		if (passed > 1) {
			printf("%f ms/frame\n", 1000.0 / double(n_frames));
			n_frames = 0;
			passed = 0;
		}
	}

	inline double getDelta() const {
		return delta;
	}

private:
	double p_time;
	double delta;
	double passed;
	size_t n_frames;
};

#endif