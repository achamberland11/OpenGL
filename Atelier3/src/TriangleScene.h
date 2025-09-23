#pragma once

#include "Scene.h"
#include "OpenGlDef.h"
#include "OpenGlResource.h"
#include "OpenGlProgram.h"

class CTriangleScene : public CScene
{
public:
	CTriangleScene();

	void Draw() override;

private:
	OpenGl::CBuffer m_vertexBuffer;
	OpenGl::CVertexArray m_vertexArray;
	OpenGl::CProgram m_program;
};
