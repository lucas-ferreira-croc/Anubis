#include "BaseLight.h"

BaseLight::BaseLight()
	: m_Color(1.0f, 1.0f, 1.0f), m_ambientIntensity(0.0f)
{
}

BaseLight::BaseLight(float r, float g, float b, float intensity)
	: m_Color(r, g, b), m_ambientIntensity(intensity)
{
}

BaseLight::BaseLight(glm::vec3 color, float intensity)
	: m_Color(color), m_ambientIntensity(intensity)
{
}

BaseLight::~BaseLight()
{
}
