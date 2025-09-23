#pragma once

#include "Scene.h"
#include "OpenGlDef.h"
#include "OpenGlResource.h"
#include "OpenGlProgram.h"
#include <glm/mat4x4.hpp>

class CCubeScene : public CScene
{
public:
	CCubeScene();

	void Update(double dt) override;
	void Draw() override;

private:
	struct Matrices
	{
		glm::mat4 worldViewProjMatrix;
	};

	OpenGl::CBuffer m_vertexBuffer;
	OpenGl::CBuffer m_indexBuffer;
	OpenGl::CBuffer m_uniformBuffer;
	OpenGl::CVertexArray m_vertexArray;
	OpenGl::CProgram m_program;

	GLuint m_matricesUniformBinding = -1;

	Matrices m_matrices;
};
