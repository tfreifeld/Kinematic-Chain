#pragma once   
#include "GLFW\glfw3.h"



class InputManager {
public:

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow*, int button, int action, int mods);
	static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void processMouse(double xpos, double ypos);
	static void rotate(float deltaX, float deltaY);
	static void translate(float deltaX, float deltaY, float newXposition , float newYposition);
	static void zoom(float yoffset);
	static void turnOnOffIKSolver();
};

	



