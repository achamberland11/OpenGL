#pragma once

#include "SceneObject.h"
#include "OpenGlDef.h"
#include "OpenGlResource.h"
#include "OpenGlProgram.h"

class CEnvironmentObject : public CSceneObject
{
public:
	CEnvironmentObject();

	void Update(double) override;
	void Draw(const VIEW_PARAMS&) override;

private:
	enum LIGHT_TYPE
	{
		DIRECTIONAL = 0,
		POINT = 1,
		SPOT = 2,
	};

	struct Matrices
	{
		glm::mat4 worldMatrix;
		glm::mat4 viewProjMatrix;
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

	OpenGl::CBuffer m_vertexBuffer;
	OpenGl::CBuffer m_indexBuffer;
	OpenGl::CTexture m_texture;
	OpenGl::CBuffer m_matricesUniformBuffer;
	OpenGl::CBuffer m_lightsUniformBuffer;
	OpenGl::CVertexArray m_vertexArray;
	OpenGl::CProgram m_program;

	Matrices m_matrices = {};
	Lights m_lights = {};
	uint32_t m_numIndices = 0;
};
