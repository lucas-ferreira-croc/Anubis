#include "BoxCollision.h"

#include <GL/glew.h>

#include <iostream>

BoxCollision::BoxCollision(CollisionVertices& positions)
	: m_Vertices(positions), m_IsColliding(false)
{
	m_Vertices.m_IndicesBuffer = {
		0, 1, 2,
		0, 1, 3,
		1, 3, 4,
		3, 4, 5,
		6, 5, 7,
		5, 6, 8,
		8, 2, 6,
		0, 8, 2,
		0, 8, 3,
		8, 3, 5,
		2, 6, 1,
		6, 1, 7
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices.m_PositionsBuffer) * m_Vertices.m_PositionsBuffer.size(), &m_Vertices.m_PositionsBuffer[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Vertices.m_IndicesBuffer[0]) * m_Vertices.m_IndicesBuffer.size(), &m_Vertices.m_IndicesBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

BoxCollision::~BoxCollision()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);
}

void BoxCollision::updatePositions(glm::mat4 wvp)
{
	glm::mat4 transform = wvp;
	
	/*m_Vertices.min = glm::vec4(m_Vertices.min, 1.0f) * wvp;
	m_Vertices.max = glm::vec4(m_Vertices.max, 1.0f) * wvp;
	*/
	
	m_Vertices.min = wvp * glm::vec4(m_Vertices.min, 1.0f);
	m_Vertices.max  = wvp * glm::vec4(m_Vertices.max, 1.0f);

}

void BoxCollision::render()
{
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_Vertices.m_IndicesBuffer.size(), GL_UNSIGNED_INT, 0);
}

bool BoxCollision::check_collision(BoxCollision& b_Box)
{
	m_IsColliding = (m_Vertices.min.x <= b_Box.m_Vertices.max.x && m_Vertices.max.x >= b_Box.m_Vertices.min.x) &&
		(m_Vertices.min.y <= b_Box.m_Vertices.max.y && m_Vertices.max.y >= b_Box.m_Vertices.min.y) &&
		(m_Vertices.min.z <= b_Box.m_Vertices.max.z && m_Vertices.max.z >= b_Box.m_Vertices.min.z);

	//std::cout << "Collision Detected: " << (m_IsColliding ? "Yes" : "No") << "\n";

	b_Box.m_IsColliding = m_IsColliding;
	return m_IsColliding;
}