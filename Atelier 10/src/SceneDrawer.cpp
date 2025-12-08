#define GLM_FORCE_SWIZZLE
#include "SceneDrawer.h"
#include <algorithm>
#include <array>

constexpr uint32_t g_shadowMapSize = 1024;

struct TransparentObject
{
	float depth;
	CSceneObject* object;
	bool operator<(const TransparentObject& rhs) const
	{
		return depth < rhs.depth;
	}
};

CSceneDrawer::CSceneDrawer()
{
	CreateShadowMap();

	m_lights.lights[0].ambientColor = glm::vec4(0.1, 0.1, 0.1, 0);
	m_lights.lights[0].diffuseColor = glm::vec4(1, 1, 1, 0);
	m_lights.lights[0].specColor = glm::vec4(0.15f, 0.15f, 0.0f, 0.0f);
	m_lights.lights[0].dir = glm::vec4(0.0f, 0.90f, 0.25f, 0);
}

void CSceneDrawer::AddObject(SceneObjectPtr object)
{
	m_objects.push_back(std::move(object));
}

void CSceneDrawer::AddPostProcess(PostProcessPtr postProcess)
{
	m_postProcesses.push_back(std::move(postProcess));
}

void CSceneDrawer::SetLightDir(glm::vec3 lightDir)
{
	m_lights.lights[0].dir = glm::vec4(lightDir.xyz(), 0);
}

void CSceneDrawer::Update(double dt)
{
	for(auto& object : m_objects)
	{
		object->Update(dt);
	}
}

void CSceneDrawer::Draw(uint32_t fbWidth, uint32_t fbHeight, const CSceneObject::VIEW_PARAMS& viewParams)
{
	glm::uvec2 fbSize = glm::uvec2(fbWidth, fbHeight);

	DoShadowPass();
	DoMainPass(fbSize, viewParams);
	DoPostProcessPass(fbSize);
}

void CSceneDrawer::DoShadowPass()
{
	constexpr float g_projectionBoxSize = 5.f;

	CSceneObject::VIEW_PARAMS shadowViewParams;
	shadowViewParams.viewMatrix = glm::lookAt(m_lights.lights[0].dir.xyz(), glm::vec3(0), glm::vec3(0, 0, 1));
	shadowViewParams.projMatrix = glm::ortho(
	    -g_projectionBoxSize / 2, g_projectionBoxSize / 2,
	    -g_projectionBoxSize / 2, g_projectionBoxSize / 2,
	    -g_projectionBoxSize / 2, g_projectionBoxSize / 2);

	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFramebuffer);
	glViewport(0, 0, g_shadowMapSize, g_shadowMapSize);

	glDepthMask(GL_TRUE);
	glClearDepthf(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	for(auto& object : m_objects)
	{
		if(object->IsTransparent()) continue;
		object->SetShadowMap(0);
		object->Draw(shadowViewParams, m_lights);
	}

	m_lights.shadowViewProjMatrix = shadowViewParams.projMatrix * shadowViewParams.viewMatrix;
}

void CSceneDrawer::DoMainPass(glm::uvec2 fbSize, const CSceneObject::VIEW_PARAMS& viewParams)
{
	if(m_mainPassSize != fbSize)
	{
		CreateMainPassResources(fbSize);
	}

	std::vector<CSceneObject*> opaqueObjects;
	std::vector<TransparentObject> transparentObjects;
	for(auto& object : m_objects)
	{
		if(object->IsTransparent())
		{
			auto worldPos = object->GetWorldPosition();
			auto viewPos = viewParams.viewMatrix * glm::vec4(worldPos, 1);
			transparentObjects.push_back({viewPos.z, object.get()});
		}
		else
		{
			opaqueObjects.push_back(object.get());
		}
	}
	std::sort(transparentObjects.begin(), transparentObjects.end());

	glBindFramebuffer(GL_FRAMEBUFFER, m_mainPassFramebuffer);
	glViewport(0, 0, fbSize.x, fbSize.y);

	glDepthMask(GL_TRUE);
	glClearDepthf(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(auto& object : opaqueObjects)
	{
		object->SetShadowMap(m_shadowMapTexture);
		object->Draw(viewParams, m_lights);
	}
	for(auto& transparentObject : transparentObjects)
	{
		transparentObject.object->SetShadowMap(m_shadowMapTexture);
		transparentObject.object->Draw(viewParams, m_lights);
	}
}

void CSceneDrawer::DoPostProcessPass(glm::uvec2 fbSize)
{
	if(m_postProcessPassSize != fbSize)
	{
		CreatePostProcessResources(fbSize);
	}

	constexpr int pingPongCount = 2;

	std::array<GLuint, pingPongCount> framebuffers =
	    {
	        m_postProcessPassFramebuffer,
	        m_mainPassFramebuffer,
	    };
	std::array<GLuint, pingPongCount> textures =
	    {
	        m_mainPassTexture,
	        m_postProcessPassTexture,
	    };

	int ppExecCount = 0;
	for(int ppIndex = 0; ppIndex < m_postProcesses.size(); ppIndex++)
	{
		auto& postProcess = m_postProcesses[ppIndex];
		if(!postProcess->IsEnabled())
		{
			continue;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[ppExecCount % pingPongCount]);
		postProcess->Draw(textures[ppExecCount % pingPongCount], fbSize);
		if(!postProcess->IsInplace())
		{
			ppExecCount++;
		}
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers[(ppExecCount + 1) % pingPongCount]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, fbSize.x, fbSize.y, 0, 0, fbSize.x, fbSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void CSceneDrawer::CreateMainPassResources(glm::uvec2 size)
{
	m_mainPassTexture = OpenGl::CTexture::Create();
	glBindTexture(GL_TEXTURE_2D, m_mainPassTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	CHECKGLERROR();

	m_mainPassDepthbuffer = OpenGl::CRenderbuffer::Create();
	glBindRenderbuffer(GL_RENDERBUFFER, m_mainPassDepthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
	CHECKGLERROR();

	m_mainPassFramebuffer = OpenGl::CFramebuffer::Create();
	glBindFramebuffer(GL_FRAMEBUFFER, m_mainPassFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_mainPassTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_mainPassDepthbuffer);
	CHECKGLERROR();

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	m_mainPassSize = size;
}

void CSceneDrawer::CreatePostProcessResources(glm::uvec2 size)
{
	m_postProcessPassTexture = OpenGl::CTexture::Create();
	glBindTexture(GL_TEXTURE_2D, m_postProcessPassTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	CHECKGLERROR();

	m_postProcessPassFramebuffer = OpenGl::CFramebuffer::Create();
	glBindFramebuffer(GL_FRAMEBUFFER, m_postProcessPassFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_postProcessPassTexture, 0);
	CHECKGLERROR();

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	m_postProcessPassSize = size;
}

void CSceneDrawer::CreateShadowMap()
{
	GLfloat borderColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};

	m_shadowMapTexture = OpenGl::CTexture::Create();
	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, g_shadowMapSize, g_shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	CHECKGLERROR();

	//Build framebuffer
	m_shadowMapFramebuffer = OpenGl::CFramebuffer::Create();
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMapTexture, 0);
	CHECKGLERROR();

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
