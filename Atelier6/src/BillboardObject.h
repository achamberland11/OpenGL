#pragma once

#include "SceneObject.h"
#include "OpenGlDef.h"
#include "OpenGlResource.h"
#include "OpenGlProgram.h"

class CBillboardObject : public CSceneObject
{
public:
	CBillboardObject();

	void SetTexture(OpenGl::CTexture);
	void Draw(const VIEW_PARAMS&) override;

private:
	struct Matrices
	{
		glm::mat4 worldMatrix;
		glm::mat4 viewProjMatrix;
	};

	OpenGl::CBuffer m_vertexBuffer;
	OpenGl::CBuffer m_indexBuffer;
	OpenGl::CTexture m_texture;
	OpenGl::CBuffer m_matricesUniform;
	OpenGl::CVertexArray m_vertexArray;
	OpenGl::CProgram m_program;

	Matrices m_matrices = {};
};
