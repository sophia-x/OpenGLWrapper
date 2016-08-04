#include "CallBackHelper.hpp"

double scroll_x, scroll_y;
GLFWwindow* scroll_window;
map<unsigned long, bool> scrolled;

void scrollCallBack(GLFWwindow *window, double x, double y) {
	scroll_x = x;
	scroll_y = y;
	scroll_window = window;
	for (auto it = scrolled.begin(); it != scrolled.end(); ++it) {
		it->second = true;
	}
}