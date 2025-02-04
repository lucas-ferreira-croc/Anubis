#include "SpotLight.h"

void SpotLight::calculate_local_direction_and_position(Transform transform)
{
	m_LocalPosition = transform.world_position_to_local_position(world_position);
	m_LocalDirection = transform.calculate_local_direction(world_direction);
}
