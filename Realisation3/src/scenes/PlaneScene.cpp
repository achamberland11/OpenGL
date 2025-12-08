#include "PlaneScene.h"
#include "OpenGlShader.h"
#include "OpenGlUtils.h"
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace
{
	struct VERTEX
	{
		glm::vec3 position;
		glm::vec2 texCoord;
	};

	enum VERTEX_ATTRIBUTES
	{
		POSITION,
		TEXCOORD,
	};

	enum UNIFORM_BINDINGS
	{
		MATRICES,
		TEXTUREPARAMS,
	};

	constexpr int g_vertexCount = 4;
	constexpr int g_indexCount = 6;
}

// clang-format off
static const VERTEX g_vertices[g_vertexCount] =
{
	{ {-1.0f, -1.0f, 5.0f}, {-1.0f, -1.0f} },
	{ {-1.0f,  1.0f, 0.0f}, {-1.0f,  1.0f} },
	{ { 1.0f, -1.0f, 5.0f}, { 1.0f, -1.0f} },
	{ { 1.0f,  1.0f, 0.0f}, { 1.0f,  1.0f} },
};

static const uint16_t g_indices[g_indexCount] =
{
	0, 1, 2,
	1, 2, 3,
};
// clang-format on

CPlaneScene::CPlaneScene()
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
		m_matricesUniform = OpenGl::CBuffer::Create();
		glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUniform);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(m_matrices), &m_matrices, GL_DYNAMIC_DRAW);
	}

	{
		m_textureParamsUniform = OpenGl::CBuffer::Create();
		glBindBuffer(GL_UNIFORM_BUFFER, m_textureParamsUniform);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(m_textureParams), &m_textureParams, GL_DYNAMIC_DRAW);
	}

	{
		auto vertShader = OpenGl::CShader::CreateFromFile(GL_VERTEX_SHADER, "../shaders/textured_v.glsl");
		auto fragShader = OpenGl::CShader::CreateFromFile(GL_FRAGMENT_SHADER, "../shaders/textured_f.glsl");

		vertShader.Compile();
		fragShader.Compile();

		m_program = OpenGl::CProgram::Create();
		m_program.AttachShader(vertShader);
		m_program.AttachShader(fragShader);
		m_program.Link();

		glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::POSITION), "a_position");
		glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::TEXCOORD), "a_texCoord");

		{
			GLuint blockIndex = glGetUniformBlockIndex(m_program, "Matrices");
			assert(blockIndex != GL_INVALID_INDEX);
			glUniformBlockBinding(m_program, blockIndex, UNIFORM_BINDINGS::MATRICES);
		}

		{
			GLuint blockIndex = glGetUniformBlockIndex(m_program, "TextureParams");
			assert(blockIndex != GL_INVALID_INDEX);
			glUniformBlockBinding(m_program, blockIndex, UNIFORM_BINDINGS::TEXTUREPARAMS);
		}
	}

	m_vertexArray = OpenGl::CVertexArray::Create();

	{
		glBindVertexArray(m_vertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

		glEnableVertexAttribArray(VERTEX_ATTRIBUTES::POSITION);
		glVertexAttribPointer(VERTEX_ATTRIBUTES::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), reinterpret_cast<GLvoid*>(offsetof(VERTEX, position)));

		glEnableVertexAttribArray(VERTEX_ATTRIBUTES::TEXCOORD);
		glVertexAttribPointer(VERTEX_ATTRIBUTES::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), reinterpret_cast<GLvoid*>(offsetof(VERTEX, texCoord)));

		glBindVertexArray(0);
	}

	m_texture = OpenGl::LoadTextureFromFile("../textures/turbulence.png");
}

void CPlaneScene::Update(double dt)
{
	CScene::Update(dt);

	float aspectRatio = static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight);

	glm::mat4 projMat = glm::perspective(glm::pi<float>() * 0.25f, aspectRatio, 0.1f, 1000.f);
	glm::mat4 viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.f));
	glm::mat4 worldMat = glm::mat4(1.0f);

	m_matrices.worldViewProjMatrix = projMat * viewMat * worldMat;

	glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUniform);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_matrices), &m_matrices, GL_DYNAMIC_DRAW);

	m_textureParams.texCoordScale = fabs(sin(m_currentTime) * 50.0f) + 1.0f;

	glBindBuffer(GL_UNIFORM_BUFFER, m_textureParamsUniform);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_textureParams), &m_textureParams, GL_DYNAMIC_DRAW);
}

void CPlaneScene::Draw()
{
	glViewport(0, 0, m_windowWidth, m_windowHeight);

	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_program);
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::MATRICES, m_matricesUniform);
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::TEXTUREPARAMS, m_textureParamsUniform);
	glBindVertexArray(m_vertexArray);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glDrawElements(GL_TRIANGLES, g_indexCount, GL_UNSIGNED_SHORT, nullptr);
}
