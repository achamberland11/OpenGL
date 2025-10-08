#include "PlaneScene.h"
#include "OpenGlShader.h"
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

	enum TEXTURE_BINDINGS
	{
		TEXTURE_1,
		TEXTURE_2,
		TEXTURE_MASK,
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
		auto vertShader = OpenGl::CShader::CreateFromFile(GL_VERTEX_SHADER, "./shaders/textured_v.glsl");
		auto fragShader = OpenGl::CShader::CreateFromFile(GL_FRAGMENT_SHADER, "./shaders/textured_f.glsl");

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
			GLuint uniformLocation = glGetUniformLocation(m_program, "s_texture1");
			if(uniformLocation != GL_INVALID_INDEX)
			{
				glProgramUniform1i(m_program, uniformLocation, TEXTURE_BINDINGS::TEXTURE_1);
			}
		}

		{
			GLuint uniformLocation = glGetUniformLocation(m_program, "s_texture2");
			if(uniformLocation != GL_INVALID_INDEX)
			{
				glProgramUniform1i(m_program, uniformLocation, TEXTURE_BINDINGS::TEXTURE_2);
			}
		}

		{
			GLuint uniformLocation = glGetUniformLocation(m_program, "s_textureMask");
			if(uniformLocation != GL_INVALID_INDEX)
			{
				glProgramUniform1i(m_program, uniformLocation, TEXTURE_BINDINGS::TEXTURE_MASK);
			}
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

OpenGl::CTexture CPlaneScene::LoadTexture(const char* path)
{
	int width = 0, height = 0, components = 0;
	stbi_uc* image = stbi_load(path, &width, &height, &components, 4);
	if(!image)
	{
		printf("Failed to load image: %s.\r\n", stbi_failure_reason());
		assert(false);
	}

	auto result = OpenGl::CTexture::Create();

	glBindTexture(GL_TEXTURE_2D, result);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(image);

	return result;
}

void CPlaneScene::Update(double dt)
{
	CScene::Update(dt);

	float aspectRatio = static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight);

	glm::mat4 projMat = glm::perspective(glm::pi<float>() * 0.25f, aspectRatio, 0.1f, 1000.f);
	glm::mat4 viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -7.f));
	glm::mat4 worldMat = glm::mat4(1.0f);

	m_matrices.worldViewProjMatrix = projMat * viewMat * worldMat;

	glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUniform);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_matrices), &m_matrices, GL_DYNAMIC_DRAW);
}

void CPlaneScene::Draw()
{
	glViewport(0, 0, m_windowWidth, m_windowHeight);

	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_program);
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::MATRICES, m_matricesUniform);
	glBindVertexArray(m_vertexArray);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_BINDINGS::TEXTURE_1);
	glBindTexture(GL_TEXTURE_2D, m_texture1);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_BINDINGS::TEXTURE_2);
	glBindTexture(GL_TEXTURE_2D, m_texture2);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_BINDINGS::TEXTURE_MASK);
	glBindTexture(GL_TEXTURE_2D, m_textureMask);
	glDrawElements(GL_TRIANGLES, g_indexCount, GL_UNSIGNED_SHORT, nullptr);
}
