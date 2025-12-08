#pragma once

#include "PostProcess.h"
#include "OpenGlDef.h"
#include "OpenGlResource.h"
#include "OpenGlProgram.h"

class CBlurPostProcess : public CPostProcess
{
public:
	CBlurPostProcess();

	void Draw(GLuint, glm::uvec2) override;

	bool IsInplace() override
	{
		return false;
	}

private:
	struct Params
	{
		int tapCount = 2;
	};

	OpenGl::CProgram m_program;
	OpenGl::CBuffer m_paramsUniformBuffer;

	Params m_params;
};
