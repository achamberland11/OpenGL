#include "GrayScalePostProcess.h"
#include "OpenGlShader.h"

namespace
{
	enum TEXTURE_BINDINGS
	{
		TEXTURE_INPUT = 0,
	};
}

CGrayScalePostProcess::CGrayScalePostProcess()
{
	auto vertShader = OpenGl::CShader::CreateFromFile(GL_VERTEX_SHADER, "../shaders/postprocess_v.glsl");
	auto fragShader = OpenGl::CShader::CreateFromFile(GL_FRAGMENT_SHADER, "../shaders/pp_gray_f.glsl");

	vertShader.Compile();
	fragShader.Compile();

	m_program = OpenGl::CProgram::Create();
	m_program.AttachShader(vertShader);
	m_program.AttachShader(fragShader);
	m_program.Link();

	m_program.SetUniform1i("s_inputTexture", TEXTURE_BINDINGS::TEXTURE_INPUT);
}

void CGrayScalePostProcess::Draw(GLuint inputTexture, glm::uvec2)
{
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_BINDINGS::TEXTURE_INPUT);
	glBindTexture(GL_TEXTURE_2D, inputTexture);
	glUseProgram(m_program);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
