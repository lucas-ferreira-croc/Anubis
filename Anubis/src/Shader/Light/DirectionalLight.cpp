#include "DirectionalLight.h"
#include <glm/gtc/matrix_transform.hpp> 

void DirectionalLight::calculate_local_direction(Transform transform)
{
	/*glm::mat3 world_mat3(world);
	glm::mat3 world_to_local = glm::transpose(world_mat3);
	
	m_LocalDirection = world_to_local * m_WorldDirection;
	m_LocalDirection = glm::normalize(m_LocalDirection);*/
	
	m_LocalDirection = transform.calculate_local_direction(m_WorldDirection);
}
