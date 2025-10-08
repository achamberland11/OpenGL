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
	OpenGl::CTexture LoadTexture(const char*);

	struct Matrices
	{
		glm::mat4 worldViewProjMatrix;
	};

	OpenGl::CBuffer m_vertexBuffer;
	OpenGl::CBuffer m_indexBuffer;
	OpenGl::CBuffer m_matricesUniform;
	OpenGl::CVertexArray m_vertexArray;
	OpenGl::CTexture m_texture1;
	OpenGl::CTexture m_texture2;
	OpenGl::CTexture m_textureMask;
	OpenGl::CProgram m_program;

	Matrices m_matrices;
};
