#include "SceneDrawer.h"
#include <algorithm>

struct TransparentObject
{
	float depth;
	CSceneObject* object;
	bool operator<(const TransparentObject& rhs) const
	{
		return depth < rhs.depth;
	}
};

void CSceneDrawer::AddObject(SceneObjectPtr object)
{
	m_objects.push_back(std::move(object));
}

void CSceneDrawer::Update(double dt)
{
	for(auto& object : m_objects)
	{
		object->Update(dt);
	}
}

void CSceneDrawer::Draw(const CSceneObject::VIEW_PARAMS& viewParams)
{
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
	for(auto& object : opaqueObjects)
	{
		object->Draw(viewParams);
	}
	for(auto& transparentObject : transparentObjects)
	{
		transparentObject.object->Draw(viewParams);
	}
}
