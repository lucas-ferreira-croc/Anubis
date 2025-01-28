#include "PointLight.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void PointLight::calculate_local_position(Transform transform)
{
	m_LocalPosition = transform.world_position_to_local_position(world_position);
}
