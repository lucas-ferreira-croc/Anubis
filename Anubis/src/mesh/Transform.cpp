#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>

void Transform::set_scale(float scale)
{
	m_scale = scale;
}

void Transform::set_rotation(float x, float y, float z)
{
	m_rotation = glm::vec3(x, y, z);
}

void Transform::set_position(float x, float y, float z)
{
	m_position = glm::vec3(x, y, z);
}

void Transform::set_position(glm::vec3 position)
{
	m_position = position;
}

void Transform::rotate(float x, float y, float z)
{
	m_rotation.x += x;
	m_rotation.x += y;
	m_rotation.x += z;
}

glm::mat4 Transform::get_matrix()
{
	glm::mat4 transform(1.0f);

	transform = glm::translate(transform, m_position);
	transform = glm::rotate(transform, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); 
	transform = glm::rotate(transform, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); 
	transform = glm::rotate(transform, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); 
	transform = glm::scale(transform, glm::vec3(m_scale, m_scale, m_scale));

	return transform;
}

glm::vec3 Transform::calculate_local_direction(glm::vec3 world_direction)
{
	glm::mat3 world_mat3(get_matrix());
	glm::mat3 world_to_local = glm::transpose(world_mat3);

	world_direction= world_to_local * world_direction;
	return glm::normalize(world_direction);
}

glm::vec3 Transform::world_position_to_local_position(glm::vec3 world_position)
{
	glm::mat4 cameraToLocalTranslation = glm::translate(glm::mat4(1.0f), -get_position());

	glm::mat4 cameraToLocalRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-get_rotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
	cameraToLocalRotation = glm::rotate(cameraToLocalRotation, glm::radians(-get_rotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
	cameraToLocalRotation = glm::rotate(cameraToLocalRotation, glm::radians(-get_rotation().x), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 ObjectToLocalTransformation = cameraToLocalRotation * cameraToLocalTranslation;

	glm::vec4 ObjectWorldPos(world_position, 1.0f);
	glm::vec4 ObjectLocalPos = ObjectToLocalTransformation * ObjectWorldPos;

	return glm::vec3(ObjectLocalPos);
}

