#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "../Mesh/Transform.h"

struct CollisionVertices
{
	std::vector<glm::vec3> m_PositionsBuffer;
	std::vector<unsigned int> m_IndicesBuffer;

	glm::vec3 min, max;
};

class BoxCollision
{
public:
	BoxCollision(CollisionVertices& positions);
	BoxCollision() {};
	~BoxCollision();

	void updatePositions(glm::mat4 wvp);
	void render();

	Transform& get_transform() { return m_Transform; }

	bool check_collision(BoxCollision& b_Box);

	bool m_IsColliding;
	CollisionVertices m_Vertices;
private:
	Transform m_Transform;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
};

