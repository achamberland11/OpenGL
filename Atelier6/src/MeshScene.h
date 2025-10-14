#pragma once

#include "Scene.h"
#include "SceneDrawer.h"
#include <cfloat>

class CMeshScene : public CScene
{
public:
	CMeshScene();

	void Update(double dt) override;
	void Draw() override;

private:
	CSceneDrawer m_drawer;
	CSceneObject::VIEW_PARAMS m_viewParams = {};

	glm::vec2 m_cameraAngle = {-0.126000, 0.208500};
	glm::vec3 m_cameraPosition = {0.366116, 0.825350, 2.265454};
	glm::dvec2 m_lastMousePosition = {DBL_MAX, DBL_MAX};
};
