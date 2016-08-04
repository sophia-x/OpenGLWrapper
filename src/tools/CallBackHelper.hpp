#ifndef CALL_BACK_HELPER
#define CALL_BACK_HELPER

#include <map>
using namespace std;

#include <GLFW/glfw3.h>

extern double scroll_x, scroll_y;
extern GLFWwindow* scroll_window;
extern map<unsigned long, bool> scrolled;

void scrollCallBack(GLFWwindow * window, double x, double y);

#endif