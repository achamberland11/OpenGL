#pragma once

class CScene
{
public:
	virtual ~CScene() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;
};
