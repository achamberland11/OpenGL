//
// Created by Antoine on 2025-12-08.
//

#pragma once
#include "OpenGlProgram.h"
#include "OpenGlResource.h"
#include "PostProcess.h"

class CVignettePostProcess : public CPostProcess
{
public:
	CVignettePostProcess();
	void Draw(GLuint, glm::uvec2) override;
	bool IsInplace() override { return false; };

private:
	struct Params
	{
		float intensity = 0.5f;
		float padding[3];
	};

	OpenGl::CProgram m_program;
	OpenGl::CBuffer m_paramsUniformBuffer;

	Params m_params;
};