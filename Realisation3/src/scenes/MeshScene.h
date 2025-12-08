#pragma once

#include "Scene.h"
#include "SceneDrawer.h"
#include <cfloat>

class CGrayScalePostProcess;
class CBlurPostProcess;
class CBlurComputePostProcess;
class CVignettePostProcess;
class CLensDistortionPostProcess;

class CMeshScene : public CScene
{
public:
	CMeshScene();

	void Update(double dt) override;
	void Draw() override;

private:
	CSceneDrawer m_drawer;
	CSceneObject::VIEW_PARAMS m_viewParams = {};

	std::shared_ptr<CGrayScalePostProcess> m_grayScalePostProcess;
	std::shared_ptr<CBlurComputePostProcess> m_blurPostProcess;
	std::shared_ptr<CVignettePostProcess> m_vignettePostProcess;
	std::shared_ptr<CLensDistortionPostProcess> m_lensDistortionPostProcess;

	glm::vec2 m_cameraAngle = {-0.126000, 0.208500};
	glm::vec3 m_cameraPosition = {0.366116, 0.825350, 2.265454};
	glm::dvec2 m_lastMousePosition = {DBL_MAX, DBL_MAX};
	glm::vec3 m_lightDir = glm::vec3(0.0f, 0.90f, 0.25f);
};
