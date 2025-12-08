#include "BlurComputePostProcess.h"
#include "OpenGlShader.h"
#include <imgui.h>

namespace
{
	enum IMAGE_BINDINGS
	{
		WORK_IMAGE = 0,
	};

	constexpr int g_workgroupSize = 32;
}

CBlurComputePostProcess::CBlurComputePostProcess()
{
	{
		auto computeShader = OpenGl::CShader::CreateFromFile(GL_COMPUTE_SHADER, "../shaders/pp_blur_horz_c.glsl");
		computeShader.Compile();

		m_horzProgram = OpenGl::CProgram::Create();
		m_horzProgram.AttachShader(computeShader);
		m_horzProgram.Link();

		m_horzProgram.SetUniform1i("s_workImage", IMAGE_BINDINGS::WORK_IMAGE);
	}

	{
		auto computeShader = OpenGl::CShader::CreateFromFile(GL_COMPUTE_SHADER, "../shaders/pp_blur_vert_c.glsl");
		computeShader.Compile();

		m_vertProgram = OpenGl::CProgram::Create();
		m_vertProgram.AttachShader(computeShader);
		m_vertProgram.Link();

		m_vertProgram.SetUniform1i("s_workImage", IMAGE_BINDINGS::WORK_IMAGE);
	}
}

void CBlurComputePostProcess::Draw(GLuint inputTexture, glm::uvec2 imageSize)
{
	ImGui::SliderInt("Blur Power", &m_passCount, 1, 10);

	glBindImageTexture(IMAGE_BINDINGS::WORK_IMAGE, inputTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

	for(int i = 0; i < m_passCount; i++)
	{
		// Horizontal pass: process rows
		glUseProgram(m_horzProgram);
		glDispatchCompute(imageSize.x / g_workgroupSize, imageSize.y, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Vertical pass: process columns
		glUseProgram(m_vertProgram);
		glDispatchCompute(imageSize.x, imageSize.y / g_workgroupSize, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
}
