#pragma once

#include <string>
#include "SceneObject.h"
#include "OpenGlDef.h"
#include "OpenGlResource.h"
#include "OpenGlProgram.h"

class CMeshObject : public CSceneObject
{
public:
	CMeshObject(const std::string&);

	void SetTexture(OpenGl::CTexture);

	void Update(double) override;
	void Draw(const VIEW_PARAMS&, const LIGHT_PARAMS&) override;

private:
	struct Matrices
	{
		glm::mat4 worldMatrix;
		glm::mat4 viewProjMatrix;
		glm::vec3 cameraPos;
	};

	OpenGl::CBuffer m_vertexBuffer;
	OpenGl::CBuffer m_indexBuffer;
	OpenGl::CTexture m_texture;
	OpenGl::CBuffer m_matricesUniformBuffer;
	OpenGl::CBuffer m_lightsUniformBuffer;
	OpenGl::CVertexArray m_vertexArray;
	OpenGl::CProgram m_program;

	Matrices m_matrices = {};
	uint32_t m_numIndices = 0;
};
