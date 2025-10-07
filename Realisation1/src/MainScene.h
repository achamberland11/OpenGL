#pragma once

#include "Scene.h"
#include "MeshScene.h"
#include "CubeScene.h"

class CMainScene : public CScene
{
public:
	CMainScene();

	void Update(double dt) override;
	void Draw() override;

private:
	CMeshScene m_meshScene;
	CCubeScene m_cubeScene;

	OpenGl::CBuffer m_sharedMatricesBuffer;
	OpenGl::CBuffer m_sharedLightsBuffer;

	struct Matrices
	{
		glm::mat4 worldMatrix;     // For MeshScene
		glm::mat4 viewProjMatrix;  // Shared
		glm::mat4 cubeWorldMatrix; // For CubeScene
	};

	struct Light
	{
		glm::vec4 ambientColor;
		glm::vec4 diffuseColor;
		glm::vec4 specColor;
		glm::vec4 dir;
		glm::vec4 pos;
		float linAttenuation;
		float quadAttenuation;
		int type;
		int padding;
	};

	struct Lights
	{
		glm::vec4 viewDir;
		alignas(16) Light lights[2];
	};

	Matrices m_matrices;
	Lights m_lights;
};
