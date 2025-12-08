#pragma once

#include "PostProcess.h"
#include "OpenGlDef.h"
#include "OpenGlResource.h"
#include "OpenGlProgram.h"

class CGrayScalePostProcess : public CPostProcess
{
public:
	CGrayScalePostProcess();

	void Draw(GLuint, glm::uvec2) override;

	bool IsInplace() override
	{
		return false;
	}

private:
	OpenGl::CProgram m_program;
};
