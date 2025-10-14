#include "MeshScene.h"
#include <glm/ext/matrix_clip_space.hpp>
#include "EnvironmentObject.h"
#include "BillboardObject.h"
#include "OpenGlUtils.h"

CMeshScene::CMeshScene()
{
	{
		auto object = std::make_shared<CEnvironmentObject>();
		object->SetScale(glm::vec3(0.000001f));
		m_drawer.AddObject(object);
	}

	{
		auto object = std::make_shared<CBillboardObject>();
		object->SetIsTransparent(true);
		object->SetScale(glm::vec3(0.10f));
		object->SetPosition(glm::vec3(0.00, 0.25, 0.08f));
		object->SetRotation(glm::quat(glm::vec3(0, glm::quarter_pi<float>(), 0)));
		object->SetTexture(OpenGl::LoadTextureFromFile("./textures/badger.png"));
		m_drawer.AddObject(object);
	}

	{
		auto object = std::make_shared<CBillboardObject>();
		object->SetIsTransparent(true);
		object->SetScale(glm::vec3(0.10f, 0.30f, 0.10f));
		object->SetTexture(OpenGl::LoadTextureFromFile("./textures/palmier.png"));
		object->SetPosition(glm::vec3(0.10, 0.45, 0.10));
		m_drawer.AddObject(object);
	}
}

void CMeshScene::Update(double dt)
{
	CScene::Update(dt);

	auto mousePosition = m_inputProvider->GetMousePosition();
	if(m_lastMousePosition.x != DBL_MAX)
	{
		glm::dvec2 delta = mousePosition - m_lastMousePosition;

		m_cameraAngle += glm::vec2(delta) * 0.0015f;
		m_cameraAngle.y = glm::clamp(m_cameraAngle.y, -glm::half_pi<float>(), glm::half_pi<float>());
	}
	m_lastMousePosition = mousePosition;

	float cosPitch = cos(-m_cameraAngle.y);
	float sinPitch = sin(-m_cameraAngle.y);
	float cosYaw = cos(-m_cameraAngle.x);
	float sinYaw = sin(-m_cameraAngle.x);

	glm::vec3 xaxis = {cosYaw, 0, -sinYaw};
	glm::vec3 yaxis = {sinYaw * sinPitch, cosPitch, cosYaw * sinPitch};
	glm::vec3 zaxis = {sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw};

	if(m_inputProvider->IsKeyPressed(IInputProvider::KEY_FRONT))
	{
		m_cameraPosition -= zaxis * 0.01f;
	}
	if(m_inputProvider->IsKeyPressed(IInputProvider::KEY_BACK))
	{
		m_cameraPosition += zaxis * 0.01f;
	}
	if(m_inputProvider->IsKeyPressed(IInputProvider::KEY_LEFT))
	{
		m_cameraPosition -= xaxis * 0.01f;
	}
	if(m_inputProvider->IsKeyPressed(IInputProvider::KEY_RIGHT))
	{
		m_cameraPosition += xaxis * 0.01f;
	}

	m_viewParams.viewMatrix = {
	    glm::vec4(xaxis.x, yaxis.x, zaxis.x, 0),
	    glm::vec4(xaxis.y, yaxis.y, zaxis.y, 0),
	    glm::vec4(xaxis.z, yaxis.z, zaxis.z, 0),
	    glm::vec4(-glm::dot(xaxis, m_cameraPosition), -glm::dot(yaxis, m_cameraPosition), -glm::dot(zaxis, m_cameraPosition), 1)};

	float aspectRatio = static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight);
	m_viewParams.projMatrix = glm::perspective(glm::pi<float>() * 0.25f, aspectRatio, 0.1f, 100000.f);

	m_drawer.Update(dt);
}

void CMeshScene::Draw()
{
	glViewport(0, 0, m_windowWidth, m_windowHeight);

	glDepthMask(GL_TRUE);
	glClearDepthf(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_drawer.Draw(m_viewParams);
}
