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
#include "Shader/Light/BaseLight.h"

int main() {

	if(!glfwInit())
	{
		std::cerr << "glfw not initialized\n";
		return 1;
	}
	
	int width = 1600;
	int height = 1200;
	GLFWwindow* window = glfwCreateWindow(width, height, "Anubis", NULL, NULL);
	if(!window)
	{
		std::cerr << "could not create window";
		glfwTerminate();
		return 1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();


	float scale = 0.0f;
	float delta = 0.0065f;
	float angle = 0.0f;
	float angle_delta = 1.0f;

	glm::mat4 identity(1.0f);

	//glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);


	glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, .1f, 100.0f);

	Transform cube_transform;
	glm::vec3 camera_pos = glm::vec3(0.0f, 0.0, 0.0f);
	glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);

	Camera camera(camera_pos, target);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(window, &camera);
	
	glfwSetKeyCallback(window, &Camera::key_callback);
	glfwSetCursorPosCallback(window, &Camera::cursor_position_callback);

	double previous_seconds = glfwGetTime();
	/*Texture texture("C:\\croc\\Anubis\\Anubis\\assets\\textures\\bad_texture.png");
	texture.load_textureA();*/

	Mesh* mesh = new Mesh();
	//if(!mesh->load("C:\\dev\\3DRenderingOpenGL\\3DRenderOpenGLPort\\3DRenderOpenGLPort\\src\\assets\\models\\Zero.obj"))
	if(!mesh->load("C:\\croc\\Anubis\\Anubis\\assets\\models\\wine_barrel.obj"))
	{
		return 0;
	}


	const char* vs_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\v_shader.glsl";
	const char* fs_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\f_shader.glsl";
	Shader shader;
	shader.create_from_file(vs_filename, fs_filename);
	BaseLight light(glm::vec3(1.0f), 1.0f);

	while(!glfwWindowShouldClose(window))
	{
		double current_seconds = glfwGetTime();
		double delta_time = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, 1);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

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

		 // fs uniforms
		shader.set_int("texture_sampler", 0);
		shader.set_light(light);
		shader.set_material(mesh->get_material());

		mesh->render();
		
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}