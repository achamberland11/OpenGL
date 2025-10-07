#pragma once

class CScene
{
public:
	virtual ~CScene() = default;

	void SetWindowSize(unsigned int width, unsigned int height)
	{
		m_windowWidth = width;
		m_windowHeight = height;
	}

	virtual void Update(double dt)
	{
		m_currentTime += dt;
	}

	virtual void Draw() = 0;

protected:
	double m_currentTime = 0;
	unsigned int m_windowWidth = 0;
	unsigned int m_windowHeight = 0;
};
