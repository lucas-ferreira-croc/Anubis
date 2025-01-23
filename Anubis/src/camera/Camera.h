#pragma once

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <GLFW/glfw3.h>

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

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
private:
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

