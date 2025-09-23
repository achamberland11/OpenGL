#include "MeshScene.h"
#include "OpenGlShader.h"
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace
{
	struct VERTEX
	{
		glm::vec3 position;
		glm::vec3 normal;
	};

	enum VERTEX_ATTRIBUTES
	{
		POSITION,
		NORMAL,
	};
}

CMeshScene::CMeshScene()
{
	Assimp::Importer importer;
	importer.ReadFile("./models/bunny.obj", aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	auto scene = importer.GetScene();
	assert(scene->HasMeshes());
	auto mesh = scene->mMeshes[0];
	assert(mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);

	std::vector<VERTEX> vertices;
	vertices.reserve(mesh->mNumVertices);
	for(int i = 0; i < mesh->mNumVertices; i++)
	{
		auto position = mesh->mVertices[i];
		auto normal = mesh->mNormals[i];
		vertices.push_back({{position.x, position.y, position.z}, {normal.x, normal.y, normal.z}});
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

	{
		m_uniformBuffer = OpenGl::CBuffer::Create();
		glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(m_matrices), &m_matrices, GL_DYNAMIC_DRAW);
	}

	{
		auto vertShader = OpenGl::CShader::CreateFromFile(GL_VERTEX_SHADER, "./shaders/proj_v.glsl");
		auto fragShader = OpenGl::CShader::CreateFromFile(GL_FRAGMENT_SHADER, "./shaders/proj_f.glsl");

		vertShader.Compile();
		fragShader.Compile();

		m_program = OpenGl::CProgram::Create();
		m_program.AttachShader(vertShader);
		m_program.AttachShader(fragShader);
		m_program.Link();

		glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::POSITION), "a_position");
		glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::NORMAL), "a_normal");

		m_matricesUniformBinding = glGetUniformBlockIndex(m_program, "Matrices");
		assert(m_matricesUniformBinding != GL_INVALID_INDEX);
		glUniformBlockBinding(m_program, m_matricesUniformBinding, 0);
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

		glBindVertexArray(0);
	}
}

void CMeshScene::Update(double dt)
{
	CScene::Update(dt);

	float aspectRatio = static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight);

	glm::mat4 projMat = glm::perspective(glm::pi<float>() * 0.25f, aspectRatio, 0.1f, 1000.f);
	glm::mat4 viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.5f));
	glm::mat4 worldMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(m_currentTime * 2), glm::vec3(0.5f, 1.0f, 0.5f));

	m_matrices.worldViewProjMatrix = projMat * viewMat * worldMat;

	glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_matrices), &m_matrices, GL_DYNAMIC_DRAW);
}

void CMeshScene::Draw()
{
	glViewport(0, 0, m_windowWidth, m_windowHeight);

	glClearDepthf(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);

	glUseProgram(m_program);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniformBuffer);
	glBindVertexArray(m_vertexArray);
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_SHORT, nullptr);
}
