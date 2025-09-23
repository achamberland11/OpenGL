#include "TriangleScene.h"
#include "OpenGlResource.h"
#include "OpenGlShader.h"
#include <glm/vec3.hpp>

struct VERTEX
{
	glm::vec3 position;
	uint32_t color;
};

static const VERTEX g_vertices[3] =
{
	{{1, 0, 0}, 0xFF000000},
	{{0, 1, 0}, 0x00FF0000},
	{{0, 0, 0}, 0x0000FF00}
};

enum VERTEX_ATTRIBUTES
{
	POSITION,
	COLOR,
};


CTriangleScene::CTriangleScene()
{
  m_vertexBuffer = OpenGl::CBuffer::Create();
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertices), g_vertices, GL_STATIC_DRAW);


	m_vertexArray = OpenGl::CVertexArray::Create();

	glBindVertexArray(m_vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glEnableVertexAttribArray(VERTEX_ATTRIBUTES::POSITION);
	glVertexAttribPointer(VERTEX_ATTRIBUTES::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), reinterpret_cast<GLvoid*>(offsetof(VERTEX, position)));
	glEnableVertexAttribArray(VERTEX_ATTRIBUTES::COLOR);
	glVertexAttribPointer(VERTEX_ATTRIBUTES::COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VERTEX), reinterpret_cast<GLvoid*>(offsetof(VERTEX, color)));
	glBindVertexArray(0);


	auto vertShader = OpenGl::CShader::CreateFromFile(GL_VERTEX_SHADER, "./shaders/simple_v.glsl");
	auto fragShader = OpenGl::CShader::CreateFromFile(GL_FRAGMENT_SHADER, "./shaders/simple_f.glsl");

	vertShader.Compile();
	fragShader.Compile();

	m_program = OpenGl::CProgram::Create();
	m_program.AttachShader(vertShader);
	m_program.AttachShader(fragShader);
	m_program.Link();

	glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::POSITION), "a_position");
	glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::COLOR), "a_color");
}

void CTriangleScene::Update()
{

}

void CTriangleScene::Draw()
{
  glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);

	glUseProgram(m_program);
	glBindVertexArray(m_vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
