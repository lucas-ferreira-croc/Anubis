#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera
{
public:
	Camera();

	void set_position(float x, float y, float  z);

	void update();

	glm::mat4 get_matrix();

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
private:
	glm::vec3 m_position;
	glm::vec3 m_target;
	glm::vec3 m_up;
	float m_speed = 1.0f;

	bool keys[1024];
};

