#ifndef TIMER_H
#define TIMER_H

#include <stdio.h>

#include <GLFW/glfw3.h>

class Timer {
public:
	Timer(): p_time{glfwGetTime()}, passed{0}, n_frames{0}, frame_rate{0} {}

	inline void tick() {
		double current_time = glfwGetTime();
		delta = current_time - p_time;
		p_time = current_time;

		passed += delta;
		n_frames ++;
		if (passed > 1) {
			printf("%f ms/frame\n", 1000.0 / double(n_frames));
			frame_rate = n_frames;
			n_frames = 0;
			passed = 0;
		}
	}

	inline double getCurrentTime() const {
		return p_time;
	}

	inline double getDelta() const {
		return delta;
	}

	inline double getFrameRate() const {
		return frame_rate;
	}

private:
	double p_time;
	double delta;
	double passed;
	size_t n_frames;
	double frame_rate;
};

#endif