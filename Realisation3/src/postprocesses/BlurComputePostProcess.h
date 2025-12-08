#pragma once

#include "PostProcess.h"
#include "OpenGlDef.h"
#include "OpenGlResource.h"
#include "OpenGlProgram.h"

class CBlurComputePostProcess : public CPostProcess
{
public:
	CBlurComputePostProcess();

	void Draw(GLuint, glm::uvec2) override;

	bool IsInplace() override
	{
		return true;
	}

private:
	OpenGl::CProgram m_horzProgram;
	OpenGl::CProgram m_vertProgram;

	int m_passCount = 2;
};
