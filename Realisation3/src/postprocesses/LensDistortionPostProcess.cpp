//
// Created by Antoine on 2025-12-08.
//

#include "LensDistortionPostProcess.h"
#include "OpenGlShader.h"
#include <imgui.h>

namespace
{
	enum IMAGE_BINDINGS
	{
		WORK_IMAGE = 0,
	};

	enum UNIFORM_BINDINGS
	{
		PARAMS,
	};

	constexpr int g_workgroupSize = 16;
}

CLensDistortionPostProcess::CLensDistortionPostProcess()
{
	m_paramsUniformBuffer = OpenGl::CBuffer::Create();

	auto computeShader = OpenGl::CShader::CreateFromFile(GL_COMPUTE_SHADER, "../shaders/pp_lens_distortion_c.glsl");
	computeShader.Compile();

	m_program = OpenGl::CProgram::Create();
	m_program.AttachShader(computeShader);
	m_program.Link();

	m_program.SetUniform1i("s_workImage", IMAGE_BINDINGS::WORK_IMAGE);
	m_program.SetUniformBlockBinding("Params", UNIFORM_BINDINGS::PARAMS);
}

void CLensDistortionPostProcess::Draw(GLuint inputTexture, glm::uvec2 imageSize)
{
	ImGui::Text("Radial Distortion:");
	ImGui::SliderFloat("K1", &m_params.k1, -1.0f, 1.0f);
	ImGui::SliderFloat("K2", &m_params.k2, -1.0f, 1.0f);

	ImGui::Text("Tangential Distortion:");
	ImGui::SliderFloat("P1", &m_params.p1, -0.5f, 0.5f);
	ImGui::SliderFloat("P2", &m_params.p2, -0.5f, 0.5f);

	glBindBuffer(GL_UNIFORM_BUFFER, m_paramsUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_params), &m_params, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::PARAMS, m_paramsUniformBuffer);
	glBindImageTexture(IMAGE_BINDINGS::WORK_IMAGE, inputTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

	glUseProgram(m_program);
	glDispatchCompute((imageSize.x + g_workgroupSize - 1) / g_workgroupSize, (imageSize.y + g_workgroupSize - 1) / g_workgroupSize, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}