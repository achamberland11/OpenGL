#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class CSceneObject
{
public:
	struct VIEW_PARAMS
	{
		glm::mat4x4 viewMatrix;
		glm::mat4x4 projMatrix;
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

	virtual void Draw(const VIEW_PARAMS&) {};

	glm::vec3 GetWorldPosition() const
	{
		return m_worldMatrix[3];
	}

protected:
	glm::vec3 m_position = {};
	glm::quat m_rotation = glm::quat(1, 0, 0, 0);
	glm::vec3 m_scale = glm::vec3(1, 1, 1);

	bool m_isTransparent = false;

	glm::mat4x4 m_worldMatrix = glm::mat4(1.0f);
};
