#pragma once

#include <memory>
#include <vector>
#include "SceneObject.h"
#include "PostProcess.h"
#include "OpenGlResource.h"

class CSceneDrawer
{
public:
	using SceneObjectPtr = std::shared_ptr<CSceneObject>;
	using PostProcessPtr = std::shared_ptr<CPostProcess>;

	CSceneDrawer();

	void AddObject(SceneObjectPtr);
	void AddPostProcess(PostProcessPtr);

	void SetLightDir(glm::vec3);

	void Update(double);
	void Draw(uint32_t, uint32_t, const CSceneObject::VIEW_PARAMS&);

private:
	void DoShadowPass();
	void DoMainPass(glm::uvec2, const CSceneObject::VIEW_PARAMS&);
	void DoPostProcessPass(glm::uvec2);

	void CreateMainPassResources(glm::uvec2);
	void CreatePostProcessResources(glm::uvec2);
	void CreateShadowMap();

	std::vector<SceneObjectPtr> m_objects;
	std::vector<PostProcessPtr> m_postProcesses;

	CSceneObject::LIGHT_PARAMS m_lights = {};

	OpenGl::CFramebuffer m_shadowMapFramebuffer;
	OpenGl::CTexture m_shadowMapTexture;

	glm::uvec2 m_mainPassSize;
	OpenGl::CFramebuffer m_mainPassFramebuffer;
	OpenGl::CTexture m_mainPassTexture;
	OpenGl::CRenderbuffer m_mainPassDepthbuffer;

	glm::uvec2 m_postProcessPassSize;
	OpenGl::CFramebuffer m_postProcessPassFramebuffer;
	OpenGl::CTexture m_postProcessPassTexture;
};
