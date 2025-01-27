#include <iostream>
#include <fstream>
#include <string>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh/Transform.h"
#include "camera/Camera.h"
#include "texture/Texture.h"
#include "Mesh/Mesh.h"
#include "Shader/Shader.h"
#include "Shader/Light/DirectionalLight.h"
#include "Display/Display.h"

int main() 
{

	int width = 1600;
	int height = 1200;
	Display display(width, height);
	display.initialize_window();



	float scale = 0.0f;
	float delta = 0.0065f;
	float angle = 0.0f;
	float angle_delta = 1.0f;

	glm::mat4 identity(1.0f);

	//glEnable(GL_CULL_FACE);

	glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, .1f, 100.0f);

	Transform cube_transform;
	glm::vec3 camera_pos = glm::vec3(0.0f, 0.0, 0.0f);
	glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);

	Camera camera(camera_pos, target);
	
	glfwSetInputMode(display.get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(display.get_window(), &camera);
	
	glfwSetKeyCallback(display.get_window(), &Camera::key_callback);
	glfwSetCursorPosCallback(display.get_window(), &Camera::cursor_position_callback);

	double previous_seconds = glfwGetTime();

	Mesh* mesh = new Mesh();
	if(!mesh->load("C:\\croc\\Anubis\\Anubis\\assets\\models\\wine_barrel.obj"))
	{
		return 0;
	}


	const char* vs_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\v_shader.glsl";
	const char* fs_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\f_shader.glsl";
	Shader shader;
	shader.create_from_file(vs_filename, fs_filename);
	DirectionalLight light;
	light.m_ambientIntensity = 0.1f;
	light.m_DiffuseIntensity = 1.0f;
	light.m_WorldDirection = glm::vec3(3.0f, 0.0f, -1.0f);

	while (!display.should_close())
	{
		double current_seconds = glfwGetTime();
		double delta_time = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		if (GLFW_PRESS == glfwGetKey(display.get_window(), GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(display.get_window(), 1);
		}

		display.clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		display.clear();
		scale += delta;
		if(std::abs(scale) >= 1.0f)
		{
			//delta *= -1.0f;
		}

		mesh->get_transform().set_position(0.0f, 0.0f, -1.0f);
		cube_transform.set_position(0.0f, 0.0f, -1.0f);

		angle += angle_delta;
		if(std::abs(glm::radians(angle)) >= glm::radians(90.0f))
		{
			//angle_delta *= -1.0f;
		}

		mesh->get_transform().set_rotation(0.0f, angle, 0.0f);
		cube_transform.set_rotation(0.0f, angle, 0.0f);

		mesh->get_transform().set_scale(0.3f);
		//cube_transform.set_scale(0.5f);
		camera.update(delta_time);

		shader.bind();
		//vs uniforms
		shader.set_mat4("model", mesh->get_transform().get_matrix());
		shader.set_mat4("view", camera.get_look_at());
		shader.set_mat4("projection", projection);

		light.calculate_local_direction(mesh->get_transform().get_matrix());

		 // fs uniforms
		shader.set_int("texture_sampler", 0);
		shader.set_directional_light(light);
		shader.set_material(mesh->get_material());

		mesh->render();
		
		display.swap_buffers();
	}

	return 0;
}