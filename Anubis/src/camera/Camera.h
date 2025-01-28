#pragma once

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <GLFW/glfw3.h>
#include "../Mesh/Transform.h"

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 target);

	glm::mat4  get_look_at();
	void calculate_look_at();
	void update(double delta_time);

	double get_xpos() {
		return xpos;
	}
	double get_ypos() {
		return ypos;
	}

	glm::vec3 get_position() { return position; }

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

	void calculate_local_position(Transform transform);
	glm::vec3 get_local_position() { return m_LocalPosition; };
private:
	glm::vec3 m_LocalPosition;

	glm::vec3 position;
	glm::vec3 target;

	float speed;
	float yaw_speed;
	float cam_yaw_angle;
	float cam_pitch_angle;

	glm::mat4 rotation;

	glm::mat4 view;

	bool keys[1024];

	double xpos;
	double ypos;

	double previous_xpos;
	double previous_ypos;
};

