#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <xutility>

Camera::Camera(glm::vec3 position, glm::vec3 target)
	: position(position), target(target), speed(5.0f), yaw_speed(10.0f), cam_yaw_angle(0.0f), cam_pitch_angle(0.0f),
	previous_xpos(0.0f), previous_ypos(0.0), xpos(0.0f), ypos(0.0f)
{
	rotation = glm::rotate(glm::mat4(1.0f), glm::radians(cam_yaw_angle), glm::vec3(0.0f, 1.0f, 0.0f));

	std::fill(std::begin(keys), std::end(keys), false);
	calculate_look_at();
}

void Camera::calculate_look_at()
{

	//rotation = glm::rotate(glm::mat4(1.0f), deg_to_radians(cam_yaw_angle), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat q = glm::angleAxis(glm::radians(cam_yaw_angle), glm::vec3(0.0f, 1.0f, 0.0f));

	//rotation = glm::rotate(rotation, deg_to_radians(cam_pitch_angle), glm::vec3(1.0f, 0.0f, 0.0f));
	q *= glm::angleAxis(glm::radians(cam_pitch_angle), glm::vec3(1.0f, 0.0f, 0.0f));

	rotation = glm::toMat4(q);
	target = glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0)));
	glm::vec3 camera_right = glm::cross(target, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 camera_up = glm::cross(camera_right, target);

	view = glm::lookAt(position, position + target, camera_up);
}

void Camera::update(double delta_time)
{
    bool cam_moved = false;

    // Vetores de direção relativos à câmera
    glm::vec3 right = glm::normalize(glm::cross(target, glm::vec3(0.0f, 1.0f, 0.0f))); // Direção direita
    glm::vec3 forward = glm::normalize(glm::vec3(target.x, 0.0f, target.z)); // Direção para frente (ignorar Y)

    if (keys[GLFW_KEY_A])
    {
        position -= right * (speed * (float)delta_time);
        cam_moved = true;
    }
    if (keys[GLFW_KEY_D])
    {
        position += right * (speed * (float)delta_time);
        cam_moved = true;
    }
    if (keys[GLFW_KEY_W])
    {
        position += forward * (speed * (float)delta_time);
        cam_moved = true;
    }
    if (keys[GLFW_KEY_S])
    {
        position -= forward * (speed * (float)delta_time);
        cam_moved = true;
    }
    if (keys[GLFW_KEY_Q])
    {
        position.y += speed * (float)delta_time;
        cam_moved = true;
    }
    if (keys[GLFW_KEY_E])
    {
        position.y -= speed * (float)delta_time;
        cam_moved = true;
    }

    if (keys[GLFW_KEY_LEFT])
    {
        cam_yaw_angle += yaw_speed * delta_time;
        cam_moved = true;
    }
    if (keys[GLFW_KEY_RIGHT])
    {
        cam_yaw_angle -= yaw_speed * delta_time;
        cam_moved = true;
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
        cam_yaw_angle -= x_displacement * sensitivity;
        previous_xpos = xpos;
        cam_moved = true;
    }

    double y_displacement = ypos - previous_ypos;
    if (abs(y_displacement) > 0.0)
    {
        cam_pitch_angle -= y_displacement * sensitivity;
        cam_pitch_angle = glm::clamp(cam_pitch_angle, -89.0f, 89.0f);
        previous_ypos = ypos;
        cam_moved = true;
    }

    if (cam_moved)
    {
        calculate_look_at();
    }
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

void Camera::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Camera* this_camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	this_camera->xpos = xpos;
	this_camera->ypos = ypos;
}

void Camera::calculate_local_position(Transform transform)
{
    m_LocalPosition = transform.world_position_to_local_position(position);
}

glm::mat4 Camera::get_look_at()
{
	return view;
}

