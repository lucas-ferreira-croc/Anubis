#pragma once
#include "PointLight.h"

class SpotLight : public PointLight
{
public:
	glm::vec3 world_direction = glm::vec3(0.0f);
	float cutoff = 0.0f;
	void calculate_local_direction_and_position(Transform transform);
	glm::vec3& get_local_direction() { return m_LocalDirection; }
private:
	glm::vec3 m_LocalDirection = glm::vec3(0.0f);
};

