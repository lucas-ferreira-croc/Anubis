#pragma once

#include <glm/glm.hpp>

class Transform
{
public:
	Transform() = default;

	void set_scale(float scale);
	
	void set_rotation(float x, float y, float z);
	glm::vec3 get_rotation() { return m_rotation; }

	void set_position(float x, float y, float z);
	void set_position(glm::vec3 position);
	glm::vec3 get_position() {
		return m_position;
	}

	void rotate(float x, float y, float z);
	glm::mat4 get_matrix();

	glm::vec3 calculate_local_direction(glm::vec3 world_direction);
	glm::vec3 world_position_to_local_position(glm::vec3 world_position);
private:
	float m_scale = 1.0f;
	glm::vec3 m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
};

