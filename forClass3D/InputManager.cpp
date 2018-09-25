#include "inputManager.h"
#include <iostream>

double xpos;
double ypos;
int lastButtonPressed = -1;

void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case GLFW_KEY_LEFT:
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
			rotate(5.0f, 0);
		break;
	case GLFW_KEY_RIGHT:
		if (action == GLFW_PRESS || GLFW_REPEAT)
			rotate(-5.0f, 0);
		break;
	case GLFW_KEY_UP:
		if (action == GLFW_PRESS || GLFW_REPEAT)
			rotate(0, 5.0f);
		break;
	case GLFW_KEY_DOWN:
		if (action == GLFW_PRESS || GLFW_REPEAT)
			rotate(0, -5.0f);
		break;
	case GLFW_KEY_SPACE:
		if (action == GLFW_PRESS)
			turnOnOffIKSolver();		
		break;
	default:
		break;
	}

}

void InputManager::cursor_position_callback(GLFWwindow* window, double newXposition, double newYposition) {

	if (lastButtonPressed == GLFW_MOUSE_BUTTON_LEFT)
		rotate((float)(xpos - newXposition), (float)(ypos - newYposition));
	else if (lastButtonPressed == GLFW_MOUSE_BUTTON_RIGHT)
		translate((float)(xpos - newXposition), (float)(ypos - newYposition), newXposition, newYposition);

	xpos = newXposition;
	ypos = newYposition;

}
void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

	if (lastButtonPressed == -1)
	{
		processMouse(xpos, ypos);
		lastButtonPressed = button;
	}
	else
		lastButtonPressed = -1;

}

void InputManager::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

	zoom((float)yoffset);
}

