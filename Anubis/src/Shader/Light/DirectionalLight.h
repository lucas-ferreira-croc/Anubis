#pragma once
#include "BaseLight.h"
#include "../../Mesh/Transform.h"
#include <glm/glm.hpp>

class DirectionalLight : public BaseLight
{
public:
	glm::vec3 m_WorldDirection;

	void calculate_local_direction(Transform transform);
	const glm::vec3& get_local_direction() const { return m_LocalDirection; }

private:
	glm::vec3 m_LocalDirection = glm::vec3(0.0f);
};

