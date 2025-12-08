#pragma once

#include "InputProvider.h"
#include "GLFW/glfw3.h"

class CGlfwInputProvider : public IInputProvider
{
public:
	CGlfwInputProvider(GLFWwindow*);

	bool IsKeyPressed(KEY_ID) override;
	glm::dvec2 GetMousePosition() override;

private:
	GLFWwindow* m_window = nullptr;
};
