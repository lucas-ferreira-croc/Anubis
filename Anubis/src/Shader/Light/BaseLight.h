#pragma once

#include <glm/glm.hpp>

class BaseLight
{
public:
	BaseLight();
	BaseLight(float r, float  g, float  b, float intensity);
	BaseLight(glm::vec3 color, float intensity);
	~BaseLight();

	glm::vec3 m_Color;
	float m_ambientIntensity;
	float m_DiffuseIntensity;
};

