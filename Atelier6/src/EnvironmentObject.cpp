#include "EnvironmentObject.h"
#include "OpenGlShader.h"
#include "OpenGlUtils.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace
{
	struct VERTEX
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	};

	enum VERTEX_ATTRIBUTES
	{
		POSITION,
		NORMAL,
		TEXCOORD,
	};

	enum UNIFORM_BINDINGS
	{
		MATRICES,
		LIGHTS,
	};
}

CEnvironmentObject::CEnvironmentObject()
{
	Assimp::Importer importer;
	importer.ReadFile("./models/Beach Scene.fbx", aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	auto scene = importer.GetScene();
	assert(scene->HasMeshes());
	auto mesh = scene->mMeshes[0];

	std::vector<VERTEX> vertices;
	vertices.reserve(mesh->mNumVertices);
	for(int i = 0; i < mesh->mNumVertices; i++)
	{
		auto position = mesh->mVertices[i];
		auto normal = mesh->mNormals[i];
		auto texCoord = mesh->mTextureCoords[0][i];
		vertices.push_back(
		    {{position.x, -position.z, position.y},
		     {normal.x, -normal.z, normal.y},
		     {texCoord.x, 1.0f - texCoord.y}});
	}

	std::vector<uint16_t> indices;
	indices.reserve(mesh->mNumFaces * 3);
	for(int i = 0; i < mesh->mNumFaces; i++)
	{
		auto face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		for(int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	m_numIndices = indices.size();

	{
		m_vertexBuffer = OpenGl::CBuffer::Create();
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	}

	{
		m_indexBuffer = OpenGl::CBuffer::Create();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
	}

	m_texture = OpenGl::LoadTextureFromFile("./textures/Atlas.png");

	m_matricesUniformBuffer = OpenGl::CBuffer::Create();
	m_lightsUniformBuffer = OpenGl::CBuffer::Create();

	{
		auto vertShader = OpenGl::CShader::CreateFromFile(GL_VERTEX_SHADER, "./shaders/environment_v.glsl");
		auto fragShader = OpenGl::CShader::CreateFromFile(GL_FRAGMENT_SHADER, "./shaders/environment_f.glsl");

		vertShader.Compile();
		fragShader.Compile();

		m_program = OpenGl::CProgram::Create();
		m_program.AttachShader(vertShader);
		m_program.AttachShader(fragShader);
		m_program.Link();

		glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::POSITION), "a_position");
		glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::NORMAL), "a_normal");
		glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::TEXCOORD), "a_texCoord");

		{
			GLint uniformBinding = glGetUniformBlockIndex(m_program, "Matrices");
			assert(uniformBinding != GL_INVALID_INDEX);
			glUniformBlockBinding(m_program, uniformBinding, UNIFORM_BINDINGS::MATRICES);
		}

		{
			GLint uniformBinding = glGetUniformBlockIndex(m_program, "Lights");
			assert(uniformBinding != GL_INVALID_INDEX);
			glUniformBlockBinding(m_program, uniformBinding, UNIFORM_BINDINGS::LIGHTS);
		}
	}

	m_vertexArray = OpenGl::CVertexArray::Create();

	{
		glBindVertexArray(m_vertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

		glEnableVertexAttribArray(VERTEX_ATTRIBUTES::POSITION);
		glVertexAttribPointer(VERTEX_ATTRIBUTES::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), reinterpret_cast<GLvoid*>(offsetof(VERTEX, position)));

		glEnableVertexAttribArray(VERTEX_ATTRIBUTES::NORMAL);
		glVertexAttribPointer(VERTEX_ATTRIBUTES::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), reinterpret_cast<GLvoid*>(offsetof(VERTEX, normal)));

		glEnableVertexAttribArray(VERTEX_ATTRIBUTES::TEXCOORD);
		glVertexAttribPointer(VERTEX_ATTRIBUTES::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), reinterpret_cast<GLvoid*>(offsetof(VERTEX, texCoord)));

		glBindVertexArray(0);
	}
}

void CEnvironmentObject::Update(double dt)
{
	CSceneObject::Update(dt);

	m_lights.lights[0].ambientColor = glm::vec4(0.4, 0.4, 0.4, 0);
	m_lights.lights[0].diffuseColor = glm::vec4(1.0, 1.0, 1.0, 0);
	m_lights.lights[0].specColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_lights.lights[0].dir = glm::vec4(0.5f, 0.5f, 0.0f, 0);
}

void CEnvironmentObject::Draw(const VIEW_PARAMS& viewParams)
{
	m_matrices.worldMatrix = m_worldMatrix;
	m_matrices.viewProjMatrix = viewParams.projMatrix * viewParams.viewMatrix;

	glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_matrices), &m_matrices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, m_lightsUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_lights), &m_lights, GL_DYNAMIC_DRAW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glEnable(GL_CULL_FACE);

	glDisable(GL_BLEND);

	glUseProgram(m_program);
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::MATRICES, m_matricesUniformBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::LIGHTS, m_lightsUniformBuffer);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glBindVertexArray(m_vertexArray);
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_SHORT, nullptr);
}
