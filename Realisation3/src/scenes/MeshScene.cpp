#include "MeshScene.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include "objects/MeshObject.h"
#include "objects/BillboardObject.h"
#include "postprocesses/GrayScalePostProcess.h"
#include "postprocesses/VignettePostProcess.h"
#include "postprocesses/LensDistortionPostProcess.h"
#include "OpenGlUtils.h"

CMeshScene::CMeshScene()
{
	{
		auto object = std::make_shared<CMeshObject>("../models/Beach Scene.fbx");
		object->SetScale(glm::vec3(0.000001f));
		object->SetTexture(OpenGl::LoadTextureFromFile("../textures/Atlas.png"));
		m_drawer.AddObject(object);
	}

	{
		auto object = std::make_shared<CBillboardObject>();
		object->SetIsTransparent(true);
		object->SetScale(glm::vec3(0.10f));
		object->SetPosition(glm::vec3(0, 0.25, 0.10));
		object->SetRotation(glm::quat(glm::vec3(0, glm::quarter_pi<float>(), 0)));
		object->SetTexture(OpenGl::LoadTextureFromFile("../textures/badger.png"));
		m_drawer.AddObject(object);
	}

	{
		auto object = std::make_shared<CBillboardObject>();
		object->SetIsTransparent(true);
		object->SetScale(glm::vec3(0.10f, 0.30f, 0.10f));
		object->SetTexture(OpenGl::LoadTextureFromFile("../textures/palmier.png"));
		object->SetPosition(glm::vec3(0.10, 0.45, 0.10));
		m_drawer.AddObject(object);
	}

	{
		m_vignettePostProcess = std::make_shared<CVignettePostProcess>();
		m_drawer.AddPostProcess(m_vignettePostProcess);
	}

	{
		m_lensDistortionPostProcess = std::make_shared<CLensDistortionPostProcess>();
		m_drawer.AddPostProcess(m_lensDistortionPostProcess);
	}
}

void CMeshScene::Update(double dt)
{
	CScene::Update(dt);

	ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);

	ImGui::Text("Camera Position: (%f, %f, %f)",
	            m_cameraPosition.x, m_cameraPosition.y, m_cameraPosition.z);
	ImGui::SliderFloat3("Light Dir", glm::value_ptr(m_lightDir), -1, 1);

	if(m_vignettePostProcess)
	{
		bool distortionEnabled = m_vignettePostProcess->IsEnabled();
		ImGui::Checkbox("Vignette Post Process", &distortionEnabled);
		m_vignettePostProcess->SetEnabled(distortionEnabled);
	}

	if(m_lensDistortionPostProcess)
	{
		bool vignetteEnabled = m_lensDistortionPostProcess->IsEnabled();
		ImGui::Checkbox("Lens Distortion Post Process", &vignetteEnabled);
		m_lensDistortionPostProcess->SetEnabled(vignetteEnabled);
	}

	m_drawer.SetLightDir(m_lightDir);

	auto mousePosition = m_inputProvider->GetMousePosition();
	auto mouseButtonLeftPressed = m_inputProvider->IsKeyPressed(IInputProvider::KEY_MOUSE_LEFT);
	if(mouseButtonLeftPressed && !ImGui::GetIO().WantCaptureMouse && (m_lastMousePosition.x != DBL_MAX))
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

	float aspectRatio = m_windowHeight ? static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight) : 1;
	m_viewParams.projMatrix = glm::perspective(glm::pi<float>() * 0.25f, aspectRatio, 0.1f, 100000.f);

	m_drawer.Update(dt);
}

void CMeshScene::Draw()
{
	m_drawer.Draw(m_windowWidth, m_windowHeight, m_viewParams);
}