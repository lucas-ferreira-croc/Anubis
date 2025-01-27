#pragma once
#include "BaseLight.h"
#include <glm/glm.hpp>

class DirectionalLight : public BaseLight
{
public:
	glm::vec3 m_WorldDirection;
	float m_DiffuseIntensity;

	void calculate_local_direction(glm::mat4 world);
	const glm::vec3& get_local_direction() const { return m_LocalDirection; }

private:
	glm::vec3 m_LocalDirection = glm::vec3(0.0f);
};

