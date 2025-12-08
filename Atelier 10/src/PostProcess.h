#pragma once

#include "OpenGlDef.h"
#include <glm/vec2.hpp>

class CPostProcess
{
public:
	virtual ~CPostProcess() = default;
	virtual void Draw(GLuint, glm::uvec2) = 0;
	virtual bool IsInplace() = 0;

	bool IsEnabled() const
	{
		return m_enabled;
	}

	void SetEnabled(bool enabled)
	{
		m_enabled = enabled;
	}

private:
	bool m_enabled = true;
};
