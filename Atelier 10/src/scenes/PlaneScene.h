#pragma once

#include "Scene.h"
#include "OpenGlDef.h"
#include "OpenGlResource.h"
#include "OpenGlProgram.h"
#include <glm/mat4x4.hpp>

class CPlaneScene : public CScene
{
public:
	CPlaneScene();

	void Update(double dt) override;
	void Draw() override;

private:
	struct Matrices
	{
		glm::mat4 worldViewProjMatrix;
	};

	struct TextureParams
	{
		float texCoordScale = 1.0f;
	};

	OpenGl::CBuffer m_vertexBuffer;
	OpenGl::CBuffer m_indexBuffer;
	OpenGl::CBuffer m_matricesUniform;
	OpenGl::CBuffer m_textureParamsUniform;
	OpenGl::CVertexArray m_vertexArray;
	OpenGl::CTexture m_texture;
	OpenGl::CProgram m_program;

	Matrices m_matrices;
	TextureParams m_textureParams;
};
