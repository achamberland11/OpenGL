#include "BlurPostProcess.h"
#include "OpenGlShader.h"
#include <imgui.h>

namespace
{
	enum TEXTURE_BINDINGS
	{
		TEXTURE_INPUT = 0,
	};

	enum UNIFORM_BINDINGS
	{
		PARAMS,
	};
}

CBlurPostProcess::CBlurPostProcess()
{
	m_paramsUniformBuffer = OpenGl::CBuffer::Create();

	auto vertShader = OpenGl::CShader::CreateFromFile(GL_VERTEX_SHADER, "../shaders/postprocess_v.glsl");
	auto fragShader = OpenGl::CShader::CreateFromFile(GL_FRAGMENT_SHADER, "../shaders/pp_blur_f.glsl");

	vertShader.Compile();
	fragShader.Compile();

	m_program = OpenGl::CProgram::Create();
	m_program.AttachShader(vertShader);
	m_program.AttachShader(fragShader);
	m_program.Link();

	m_program.SetUniform1i("s_inputTexture", TEXTURE_BINDINGS::TEXTURE_INPUT);
	m_program.SetUniformBlockBinding("Params", UNIFORM_BINDINGS::PARAMS);
}

void CBlurPostProcess::Draw(GLuint inputTexture, glm::uvec2)
{
	ImGui::SliderInt("Blur Power", &m_params.tapCount, 1, 10);

	glBindBuffer(GL_UNIFORM_BUFFER, m_paramsUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_params), &m_params, GL_DYNAMIC_DRAW);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::PARAMS, m_paramsUniformBuffer);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_BINDINGS::TEXTURE_INPUT);
	glBindTexture(GL_TEXTURE_2D, inputTexture);
	glUseProgram(m_program);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
