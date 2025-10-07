#include "CubeScene.h"
#include "OpenGlShader.h"
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace
{
	struct VERTEX
	{
		glm::vec3 position;
		uint32_t color;
	};

	enum VERTEX_ATTRIBUTES
	{
		POSITION,
		COLOR,
	};

	enum UNIFORM_BINDINGS
	{
		MATRICES,
		LIGHTS,
	};
}

// clang-format off
static const VERTEX g_vertices[8] =
{
	{ {-1.0f,  1.0f, -1.0f}, 0xFF800000 },
	{ { 1.0f,  1.0f, -1.0f}, 0xFFFF0000 },
	{ { 1.0f,  1.0f,  1.0f}, 0xFFFF80FF },
	{ {-1.0f,  1.0f,  1.0f}, 0xFFFFFFFF },

	{ {-1.0f, -1.0f,  1.0f}, 0xFFFFFFFF },
	{ { 1.0f, -1.0f,  1.0f}, 0xFFFFFFFF },
	{ { 1.0f, -1.0f, -1.0f}, 0xFFFFFFFF },
	{ {-1.0f, -1.0f, -1.0f}, 0xFFFFFFFF },
};

static const uint16_t g_indices[36] =
{
	0, 1, 2,
	0, 2, 3,

	4, 5, 6,
	4, 6, 7,

	3, 2, 5,
	3, 5, 4,

	2, 1, 6,
	2, 6, 5,

	1, 7, 6,
	1, 0, 7,

	0, 3, 4,
	0, 4, 7
};
// clang-format on

CCubeScene::CCubeScene()
{
	{
		m_vertexBuffer = OpenGl::CBuffer::Create();
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertices), g_vertices, GL_STATIC_DRAW);
	}

	{
		m_indexBuffer = OpenGl::CBuffer::Create();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_indices), g_indices, GL_STATIC_DRAW);
	}

	{
		m_uniformBuffer = OpenGl::CBuffer::Create();
		glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(m_matrices), &m_matrices, GL_DYNAMIC_DRAW);
	}

	m_lightsUniformBuffer = OpenGl::CBuffer::Create();

	{
		auto vertShader = OpenGl::CShader::CreateFromFile(GL_VERTEX_SHADER, "./shaders/proj_v.glsl");
		auto fragShader = OpenGl::CShader::CreateFromFile(GL_FRAGMENT_SHADER, "./shaders/proj_f.glsl");

		vertShader.Compile();
		fragShader.Compile();

		m_program = OpenGl::CProgram::Create();
		m_program.AttachShader(vertShader);
		m_program.AttachShader(fragShader);

		glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::POSITION), "a_position");
		glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::COLOR), "a_color");
		
		m_program.Link();

		{
			m_matricesUniformBinding = glGetUniformBlockIndex(m_program, "Matrices");
			assert(m_matricesUniformBinding != GL_INVALID_INDEX);
			glUniformBlockBinding(m_program, m_matricesUniformBinding, 0);
		}
	}

	m_vertexArray = OpenGl::CVertexArray::Create();

	{
		glBindVertexArray(m_vertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

		glEnableVertexAttribArray(VERTEX_ATTRIBUTES::POSITION);
		glVertexAttribPointer(VERTEX_ATTRIBUTES::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), reinterpret_cast<GLvoid*>(offsetof(VERTEX, position)));

		glEnableVertexAttribArray(VERTEX_ATTRIBUTES::COLOR);
		glVertexAttribPointer(VERTEX_ATTRIBUTES::COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VERTEX), reinterpret_cast<GLvoid*>(offsetof(VERTEX, color)));

		glBindVertexArray(0);
	}

	m_matricesUniformBinding = glGetUniformBlockIndex(m_program, "Matrices");
	assert(m_matricesUniformBinding != GL_INVALID_INDEX);
	glUniformBlockBinding(m_program, m_matricesUniformBinding, 0);
}

void CCubeScene::Update(double dt)
{
	CScene::Update(dt);

	float aspectRatio = static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight);

	glm::mat4 projMat = glm::perspective(glm::pi<float>() * 0.25f, aspectRatio, 0.1f, 1000.f);
	glm::mat4 viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -5.f));
	glm::mat4 worldMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(m_currentTime * 2), glm::vec3(0.5f, 1.0f, 0.5f));
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

	m_matrices.worldViewProjMatrix = projMat * viewMat * scaleMat * worldMat;

	glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_matrices), &m_matrices, GL_DYNAMIC_DRAW);

	m_lights.viewDir = viewMat[2];

	m_lights.lights[0].type = LIGHT_TYPE::DIRECTIONAL;
	m_lights.lights[0].ambientColor = glm::vec4(0.1, 0.1, 0.2, 0);
	m_lights.lights[0].diffuseColor = glm::vec4(1.0, 0.0, 0.0, 0);
	m_lights.lights[0].specColor = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	m_lights.lights[0].dir = glm::vec4(sin(m_currentTime * 25.f), 0, cos(m_currentTime * 25.f), 0);

	m_lights.lights[1].diffuseColor = glm::vec4(0, 1, 0, 0);
	m_lights.lights[1].specColor = glm::vec4(0, 1, 0, 0);
	m_lights.lights[1].pos = glm::vec4(0.0f, 0.5, 0.75f, 0.0f);
	m_lights.lights[1].type = LIGHT_TYPE::POINT;
	m_lights.lights[1].linAttenuation = 2;
	m_lights.lights[1].quadAttenuation = 1;

	glBindBuffer(GL_UNIFORM_BUFFER, m_lightsUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_lights), &m_lights, GL_DYNAMIC_DRAW);
}

void CCubeScene::Draw()
{
	/*
	glViewport(0, 0, m_windowWidth, m_windowHeight);

	glClearDepthf(1.0f);
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	*/

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	glUseProgram(m_program);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniformBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::LIGHTS, m_lightsUniformBuffer);
	glBindVertexArray(m_vertexArray);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
}
