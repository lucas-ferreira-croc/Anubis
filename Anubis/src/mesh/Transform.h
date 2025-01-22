#pragma once

#include <glm/glm.hpp>

class Transform
{
public:
	Transform() = default;

	void set_scale(float scale);
	void set_rotation(float x, float y, float z);
	void set_position(float x, float y, float z);

	void rotate(float x, float y, float z);
	glm::mat4 get_matrix();

private:
	float m_scale = 1.0f;
	glm::vec3 m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
};

