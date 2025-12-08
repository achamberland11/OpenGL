#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <OpenGlDef.h>

class CSceneObject
{
public:
	enum LIGHT_TYPE
	{
		DIRECTIONAL = 0,
		POINT = 1,
		SPOT = 2,
	};

	struct Light
	{
		glm::vec4 ambientColor;
		glm::vec4 diffuseColor;
		glm::vec4 specColor;
		glm::vec4 dir;
		glm::vec4 pos;
		float linAttenuation;
		float quadAttenuation;
		int type;
		int padding;
	};

	struct LIGHT_PARAMS
	{
		alignas(16) Light lights[2];
		glm::mat4x4 shadowViewProjMatrix;
	};

	struct VIEW_PARAMS
	{
		glm::mat4x4 viewMatrix;
		glm::mat4x4 projMatrix;
		glm::vec3 cameraPosition;
	};

	virtual ~CSceneObject() = default;

	void SetPosition(const glm::vec3& position)
	{
		m_position = position;
	}

	void SetRotation(const glm::quat& rotation)
	{
		m_rotation = rotation;
	}

	void SetScale(const glm::vec3& scale)
	{
		m_scale = scale;
	}

	bool IsTransparent() const
	{
		return m_isTransparent;
	}

	void SetIsTransparent(bool isTransparent)
	{
		m_isTransparent = isTransparent;
	}

	virtual void Update(double)
	{
		m_worldMatrix = glm::translate(glm::mat4(1.0f), m_position);
		m_worldMatrix *= glm::mat4_cast(m_rotation);
		m_worldMatrix = glm::scale(m_worldMatrix, m_scale);
	}

	virtual void Draw(const VIEW_PARAMS&, const LIGHT_PARAMS&) {};

	glm::vec3 GetWorldPosition() const
	{
		return m_worldMatrix[3];
	}

	void SetShadowMap(GLuint shadowMapTexture)
	{
		m_shadowMapTexture = shadowMapTexture;
	}

protected:
	glm::vec3 m_position = {};
	glm::quat m_rotation = glm::quat(1, 0, 0, 0);
	glm::vec3 m_scale = glm::vec3(1, 1, 1);

	bool m_isTransparent = false;

	glm::mat4x4 m_worldMatrix = glm::mat4(1.0f);
	GLuint m_shadowMapTexture = 0;
};
