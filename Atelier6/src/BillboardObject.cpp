#include "BillboardObject.h"
#include "OpenGlShader.h"

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
	};

	constexpr int g_vertexCount = 4;
	constexpr int g_indexCount = 6;
}

// clang-format off
static const VERTEX g_vertices[g_vertexCount] =
{
	{ {-1.0f, -1.0f, 0.0f}, { 0.0f,  1.0f} },
	{ {-1.0f,  1.0f, 0.0f}, { 0.0f,  0.0f} },
	{ { 1.0f, -1.0f, 0.0f}, { 1.0f,  1.0f} },
	{ { 1.0f,  1.0f, 0.0f}, { 1.0f,  0.0f} },
};

static const uint16_t g_indices[g_indexCount] =
{
	0, 1, 2,
	1, 2, 3,
};
// clang-format on

CBillboardObject::CBillboardObject()
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
		auto vertShader = OpenGl::CShader::CreateFromFile(GL_VERTEX_SHADER, "./shaders/billboard_v.glsl");
		auto fragShader = OpenGl::CShader::CreateFromFile(GL_FRAGMENT_SHADER, "./shaders/billboard_f.glsl");

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
}

void CBillboardObject::SetTexture(OpenGl::CTexture texture)
{
	m_texture = std::move(texture);
}

void CBillboardObject::Draw(const VIEW_PARAMS& viewParams)
{
	m_matrices.worldMatrix = m_worldMatrix;
	m_matrices.viewProjMatrix = viewParams.projMatrix * viewParams.viewMatrix;

	glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUniform);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_matrices), &m_matrices, GL_DYNAMIC_DRAW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m_program);
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::MATRICES, m_matricesUniform);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glBindVertexArray(m_vertexArray);
	glDrawElements(GL_TRIANGLES, g_indexCount, GL_UNSIGNED_SHORT, nullptr);
}
