#include "MainScene.h"
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

	enum UNIFORM_BINDINGS
	{
		MATRICES,
		LIGHTS,
	};

	// clang-format off
	// Cube vertices with normals - each face has its own vertices for proper normals
	static const VERTEX g_cubeVertices[24] =
	{
		// Top face (y = 1)
		{ {-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },
		{ { 1.0f,  1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },
		{ { 1.0f,  1.0f,  1.0f}, {0.0f, 1.0f, 0.0f} },
		{ {-1.0f,  1.0f,  1.0f}, {0.0f, 1.0f, 0.0f} },

		// Bottom face (y = -1)
		{ {-1.0f, -1.0f,  1.0f}, {0.0f, -1.0f, 0.0f} },
		{ { 1.0f, -1.0f,  1.0f}, {0.0f, -1.0f, 0.0f} },
		{ { 1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f} },
		{ {-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f} },

		// Front face (z = 1)
		{ {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f} },
		{ { 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f} },
		{ { 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f} },
		{ {-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f} },

		// Right face (x = 1)
		{ { 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f, 0.0f} },
		{ { 1.0f,  1.0f, -1.0f}, {1.0f, 0.0f, 0.0f} },
		{ { 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f} },
		{ { 1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 0.0f} },

		// Back face (z = -1)
		{ { 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, -1.0f} },
		{ {-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, -1.0f} },
		{ {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f} },
		{ { 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f} },

		// Left face (x = -1)
		{ {-1.0f,  1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f} },
		{ {-1.0f,  1.0f,  1.0f}, {-1.0f, 0.0f, 0.0f} },
		{ {-1.0f, -1.0f,  1.0f}, {-1.0f, 0.0f, 0.0f} },
		{ {-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f} },
	};

	static const uint16_t g_cubeIndices[36] =
	{
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23
	};
	// clang-format on
}

CMainScene::CMainScene()
{
	// Load FBX mesh
	Assimp::Importer importer;
	importer.ReadFile("../models/cat.fbx", aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	auto scene = importer.GetScene();
	if(!scene || !scene->mRootNode)
	{
		printf("Failed to load model: %s\n", importer.GetErrorString());
		assert(false);
	}
	assert(scene->HasMeshes());

	auto mesh = scene->mMeshes[0];

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

	// Create FBX mesh buffers
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

	m_matricesUniformBuffer = OpenGl::CBuffer::Create();
	m_lightsUniformBuffer = OpenGl::CBuffer::Create();

	// Create FBX mesh program
	{
		auto vertShader = OpenGl::CShader::CreateFromFile(GL_VERTEX_SHADER, "../shaders/light_v.glsl");
		auto fragShader = OpenGl::CShader::CreateFromFile(GL_FRAGMENT_SHADER, "../shaders/light_f.glsl");

		vertShader.Compile();
		fragShader.Compile();

		m_program = OpenGl::CProgram::Create();
		m_program.AttachShader(vertShader);
		m_program.AttachShader(fragShader);

		glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::POSITION), "a_position");
		glBindAttribLocation(m_program, static_cast<GLuint>(VERTEX_ATTRIBUTES::NORMAL), "a_normal");

		m_program.Link();

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

	// Create FBX mesh vertex array
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

	// Create cube buffers
	{
		m_cubeVertexBuffer = OpenGl::CBuffer::Create();
		glBindBuffer(GL_ARRAY_BUFFER, m_cubeVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_cubeVertices), g_cubeVertices, GL_STATIC_DRAW);
	}

	{
		m_cubeIndexBuffer = OpenGl::CBuffer::Create();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_cubeIndices), g_cubeIndices, GL_STATIC_DRAW);
	}

	m_cubeMatricesUniformBuffer = OpenGl::CBuffer::Create();

	// Create cube program (using lighting shaders)
	{
		auto vertShader = OpenGl::CShader::CreateFromFile(GL_VERTEX_SHADER, "../shaders/proj_v.glsl");
		auto fragShader = OpenGl::CShader::CreateFromFile(GL_FRAGMENT_SHADER, "../shaders/proj_f.glsl");

		vertShader.Compile();
		fragShader.Compile();

		m_cubeProgram = OpenGl::CProgram::Create();
		m_cubeProgram.AttachShader(vertShader);
		m_cubeProgram.AttachShader(fragShader);

		glBindAttribLocation(m_cubeProgram, static_cast<GLuint>(VERTEX_ATTRIBUTES::POSITION), "a_position");
		glBindAttribLocation(m_cubeProgram, static_cast<GLuint>(VERTEX_ATTRIBUTES::NORMAL), "a_normal");

		m_cubeProgram.Link();

		{
			GLint uniformBinding = glGetUniformBlockIndex(m_cubeProgram, "Matrices");
			assert(uniformBinding != GL_INVALID_INDEX);
			glUniformBlockBinding(m_cubeProgram, uniformBinding, UNIFORM_BINDINGS::MATRICES);
		}

		{
			GLint uniformBinding = glGetUniformBlockIndex(m_cubeProgram, "Lights");
			assert(uniformBinding != GL_INVALID_INDEX);
			glUniformBlockBinding(m_cubeProgram, uniformBinding, UNIFORM_BINDINGS::LIGHTS);
		}
	}

	// Create cube vertex array
	m_cubeVertexArray = OpenGl::CVertexArray::Create();

	{
		glBindVertexArray(m_cubeVertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, m_cubeVertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIndexBuffer);

		glEnableVertexAttribArray(VERTEX_ATTRIBUTES::POSITION);
		glVertexAttribPointer(VERTEX_ATTRIBUTES::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), reinterpret_cast<GLvoid*>(offsetof(VERTEX, position)));

		glEnableVertexAttribArray(VERTEX_ATTRIBUTES::NORMAL);
		glVertexAttribPointer(VERTEX_ATTRIBUTES::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), reinterpret_cast<GLvoid*>(offsetof(VERTEX, normal)));

		glBindVertexArray(0);
	}
}

void CMainScene::Update(double dt)
{
	CScene::Update(dt);

	float aspectRatio = static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight);

	glm::mat4 projMat = glm::perspective(glm::pi<float>() * 0.25f, aspectRatio, 0.1f, 1000.f);
	glm::mat4 viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f * sin(m_currentTime * 1.25f), -5.0f));

	// Update FBX mesh matrices
	{
		glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.075f));

		// Rotate mesh, le flip est là pour tourner le mesh dans le bon sens (par défaut il est à l'envers)
		glm::mat4 animatedRot = glm::rotate(glm::mat4(1.0f), static_cast<float>(m_currentTime * 8), glm::vec3(0.f, 0.f, 0.5f));
		glm::mat4 flipMat = glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(-0.5f, 0.65f, 0.75f));
		glm::mat4 worldMat = flipMat * animatedRot;

		m_matrices.worldMatrix = scaleMat * worldMat;
		m_matrices.viewProjMatrix = projMat * viewMat;

		glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(m_matrices), &m_matrices, GL_DYNAMIC_DRAW);
	}

	// Update cube matrices
	{
		glm::mat4 projCubeMat = glm::perspective(glm::pi<float>() * 0.25f, aspectRatio, 0.1f, 1000.f);
		glm::mat4 viewCubeMat = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -5.f));

		glm::mat4 cubeRotMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(0.f), glm::vec3(0.5f, 0.1f, 0.5f));
		glm::mat4 cubeScaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

		glm::mat4 cubeWorldMat = cubeScaleMat * cubeRotMat;

		m_cubeMatrices.worldViewProjMatrix = projCubeMat * viewCubeMat * cubeWorldMat;
		// m_cubeMatrices.worldMatrix = cubeWorldMat;
		// m_cubeMatrices.viewProjMatrix = projCubeMat * viewCubeMat;

		glBindBuffer(GL_UNIFORM_BUFFER, m_cubeMatricesUniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(m_cubeMatrices), &m_cubeMatrices, GL_DYNAMIC_DRAW);
	}

	// Update lights
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

void CMainScene::Draw()
{
	glViewport(0, 0, m_windowWidth, m_windowHeight);

	glClearDepthf(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	// Draw FBX mesh
	glUseProgram(m_program);
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::MATRICES, m_matricesUniformBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::LIGHTS, m_lightsUniformBuffer);
	glBindVertexArray(m_vertexArray);
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_SHORT, nullptr);

	// Draw cube
	glUseProgram(m_cubeProgram);
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::MATRICES, m_cubeMatricesUniformBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDINGS::LIGHTS, m_lightsUniformBuffer);
	glBindVertexArray(m_cubeVertexArray);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
}