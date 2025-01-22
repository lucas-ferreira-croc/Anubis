#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <xutility>
#include <iostream>

Camera::Camera()
	: m_position(0.0f, 0.0f, 0.0f), m_target(0.0f, 0.0f, 1.0f), m_up(0.0f, 1.0f, 0.0f)
{
	std::fill(std::begin(keys), std::end(keys), false);
}

void Camera::set_position(float x, float y, float z)
{
	m_position = glm::vec3(x, y, z);
}

void Camera::update()
{
	if (keys[GLFW_KEY_W])
	{
		m_position += (m_target * m_speed);
	}
	if (keys[GLFW_KEY_S])
	{
		m_position -= (m_target * m_speed);
	}

	if (keys[GLFW_KEY_A])
	{
		glm::vec3 left = glm::cross(m_target, m_up);
		left = glm::normalize(left);
		left *= m_speed;
		m_position += left;
	}
	if (keys[GLFW_KEY_D])
	{
		glm::vec3 right = glm::cross(m_up, m_target);
		right = glm::normalize(right);
		right *= m_speed;
		m_position += right;
	}

	if (keys[GLFW_KEY_Q])
	{
		m_position.y += m_speed;
	}
	if (keys[GLFW_KEY_E])
	{
		m_position.y -= m_speed;
	}
		
	if (keys[GLFW_KEY_1])
	{
		m_speed += 0.1f;
		std::cout << "speed changed to " << m_speed << "\n";
	}
	if (keys[GLFW_KEY_2])
	{
		m_speed -= 0.1f;
		if(m_speed < 0.1f)
			m_speed = 0.1f;
		std::cout << "speed changed to " << m_speed << "\n";
	}


		/*if (keys[GLFW_KEY_LEFT])
		{
			cam_yaw_angle += yaw_m_speed;
		}
		if (keys[GLFW_KEY_RIGHT])
		{
			cam_yaw_angle -= yaw_m_speed;
		}

		if (previous_xpos == 0.0 && previous_ypos == 0.0)
		{
			previous_xpos = xpos;
			previous_ypos = ypos;
		}

		float sensitivity = 0.1f;
		double x_displacement = xpos - previous_xpos;
		if (abs(x_displacement) > 0.0)
		{
			cam_yaw_angle += x_displacement * sensitivity;
			previous_xpos = xpos;
			cam_moved = true;
		}

		double y_displacement = ypos - previous_ypos;
		if (abs(y_displacement) > 0.0)
		{
			cam_pitch_angle += y_displacement * sensitivity;
			cam_pitch_angle = glm::clamp(cam_pitch_angle, -89.0f, 89.0f);
			previous_ypos = ypos;
			cam_moved = true;
		}

		if (cam_moved)
		{
			calculate_look_at();
		}*/
}

glm::mat4 Camera::get_matrix()
{
	glm::vec3 camera_right = glm::cross(m_target, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 camera_up = glm::cross(camera_right, glm::vec3(0.0f, 1.0f, 0.0f));
	//return glm::lookAt(m_position, m_position + m_target, camera_up);
	return glm::lookAt(m_position, m_position + m_target, m_up);
}

void Camera::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Camera* this_camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			this_camera->keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			this_camera->keys[key] = false;

		}
	}
}