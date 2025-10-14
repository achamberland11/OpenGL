#include "GlfwInputProvider.h"

CGlfwInputProvider::CGlfwInputProvider(GLFWwindow* window)
    : m_window(window)
{
}

bool CGlfwInputProvider::IsKeyPressed(KEY_ID keyId)
{
	switch(keyId)
	{
	case KEY_FRONT:
		return glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS;
	case KEY_BACK:
		return glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS;
	case KEY_LEFT:
		return glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS;
	case KEY_RIGHT:
		return glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS;
	default:
		return false;
	}
}

glm::dvec2 CGlfwInputProvider::GetMousePosition()
{
	double xpos = 0, ypos = 0;
	glfwGetCursorPos(m_window, &xpos, &ypos);
	return glm::dvec2(xpos, ypos);
}
