#include "MainScene.h"
#include <glm/ext.hpp>

CMainScene::CMainScene()
{
	m_sharedMatricesBuffer = OpenGl::CBuffer::Create();
	m_sharedLightsBuffer = OpenGl::CBuffer::Create();

	m_meshScene.SetSharedUniformBuffers(m_sharedMatricesBuffer, m_sharedLightsBuffer);
	m_cubeScene.SetSharedUniformBuffers(m_sharedMatricesBuffer, m_sharedLightsBuffer);
}

void CMainScene::Update(double dt)
{
	CScene::Update(dt);

	float aspectRatio = static_cast<float>(m_windowWidth) / m_windowHeight;

	glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 100.f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));

	// Mesh world transform
	glm::mat4 meshWorld = glm::rotate(glm::mat4(1.0f), static_cast<float>(m_currentTime), glm::vec3(0, 1, 0));
	// Cube world transform
	glm::mat4 cubeWorld = glm::translate(glm::mat4(1.0f), glm::vec3(2, 0, 0)) *
	                      glm::rotate(glm::mat4(1.0f), static_cast<float>(m_currentTime * 2), glm::vec3(0.5f, 1, 0));

	m_matrices.worldMatrix = meshWorld;
	m_matrices.viewProjMatrix = proj * view;
	m_matrices.cubeWorldMatrix = cubeWorld;

	glBindBuffer(GL_UNIFORM_BUFFER, m_sharedMatricesBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_matrices), &m_matrices, GL_DYNAMIC_DRAW);

	m_lights.viewDir = glm::vec4(view[2]);

	m_lights.lights[0].type = 0;
	m_lights.lights[0].ambientColor = glm::vec4(0.1f, 0.1f, 0.2f, 0.0f);
	m_lights.lights[0].diffuseColor = glm::vec4(1.0, 0.0, 0.0, 0);
	m_lights.lights[0].specColor = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	m_lights.lights[0].dir = glm::vec4(sin(m_currentTime * 25.f), 0, cos(m_currentTime * 25.f), 0);

	m_lights.lights[1].diffuseColor = glm::vec4(0, 1, 0, 0);
	m_lights.lights[1].specColor = glm::vec4(0, 1, 0, 0);
	m_lights.lights[1].pos = glm::vec4(0.0f, 0.5f, 0.75f, 0.0f);
	m_lights.lights[1].type = 1;
	m_lights.lights[1].linAttenuation = 2.0f;
	m_lights.lights[1].quadAttenuation = 1.0f;

	glBindBuffer(GL_UNIFORM_BUFFER, m_sharedLightsBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(m_lights), &m_lights, GL_DYNAMIC_DRAW);

	// Forward update (they won't touch lights)
	m_meshScene.SetWindowSize(m_windowWidth, m_windowHeight);
	m_cubeScene.SetWindowSize(m_windowWidth, m_windowHeight);

	m_meshScene.Update(dt);
	m_cubeScene.Update(dt);
}

void CMainScene::Draw()
{
	glViewport(0, 0, m_windowWidth, m_windowHeight);
	glClearDepthf(1.0f);
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);

	m_meshScene.Draw();
	m_cubeScene.Draw();
}
