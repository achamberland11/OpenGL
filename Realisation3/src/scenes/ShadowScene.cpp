#include "ShadowScene.h"
#include <glm/ext/matrix_clip_space.hpp>
#include "OpenGlUtils.h"
#include "objects/MeshObject.h"

CShadowScene::CShadowScene()
{
	{
		m_bunnyObject = std::make_shared<CMeshObject>("./models/bunny.obj");
		m_bunnyObject->SetScale(glm::vec3(5.f));
		m_bunnyObject->SetRotation(glm::quat(glm::vec3(-glm::half_pi<float>(), 0, 0)));
		m_drawer.AddObject(m_bunnyObject);
	}

	{
		auto object = std::make_shared<CMeshObject>("./models/Prism.fbx");
		m_drawer.AddObject(object);
	}
}

void CShadowScene::Update(double dt)
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
	m_viewParams.cameraPosition = m_cameraPosition;

	float aspectRatio = static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight);
	m_viewParams.projMatrix = glm::perspective(glm::pi<float>() * 0.25f, aspectRatio, 0.1f, 100.f);

	m_bunnyObject->SetPosition(glm::vec3(sin(m_currentTime), 0.5, cos(m_currentTime)));

	m_drawer.Update(dt);
}

void CShadowScene::Draw()
{
	m_drawer.Draw(m_windowWidth, m_windowHeight, m_viewParams);
}
