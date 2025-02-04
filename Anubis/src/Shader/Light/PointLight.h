#pragma once
#include "BaseLight.h"
#include "../../Mesh/Transform.h"

struct LightAttenuation
{
	float Constant = 1.0f;
	float Linear = 0.0f;
	float Exp = 0.0f;
};

class PointLight : public BaseLight
{
public:
	glm::vec3 world_position = glm::vec3(0.0f);
	LightAttenuation attenuation;

	void calculate_local_position(Transform transform);
	glm::vec3& get_local_position() { return m_LocalPosition; };
protected:
	glm::vec3 m_LocalPosition = glm::vec3(0.0f);
};

