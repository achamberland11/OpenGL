#pragma once

#include <algorithm>
#include <glm/vec2.hpp>

class IInputProvider
{
public:
	enum KEY_ID
	{
		KEY_FRONT,
		KEY_BACK,
		KEY_LEFT,
		KEY_RIGHT
	};

	virtual bool IsKeyPressed(KEY_ID) = 0;
	virtual glm::dvec2 GetMousePosition() = 0;
};
