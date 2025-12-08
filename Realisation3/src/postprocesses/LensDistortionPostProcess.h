//
// Created by Antoine on 2025-12-08.
//

#pragma once

#include "PostProcess.h"
#include "OpenGlDef.h"
#include "OpenGlResource.h"
#include "OpenGlProgram.h"

class CLensDistortionPostProcess : public CPostProcess
{
public:
	CLensDistortionPostProcess();

	void Draw(GLuint, glm::uvec2) override;

	bool IsInplace() override { return true; }

private:
	struct Params
	{
		float k1 = 0.2f;
		float k2 = 0.0f;
		float p1 = 0.0f;
		float p2 = 0.0f;
	};


	OpenGl::CProgram m_program;
	OpenGl::CBuffer m_paramsUniformBuffer;

	Params m_params;
};
