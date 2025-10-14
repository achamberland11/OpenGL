#pragma once

#include <memory>
#include <vector>
#include <glm/mat4x4.hpp>
#include "SceneObject.h"

class CSceneDrawer
{
public:
	typedef std::shared_ptr<CSceneObject> SceneObjectPtr;

	void AddObject(SceneObjectPtr);
	void Update(double);
	void Draw(const CSceneObject::VIEW_PARAMS&);

private:
	std::vector<SceneObjectPtr> m_objects;
};
