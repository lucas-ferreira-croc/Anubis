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

const double pi = 3.14159265358979323846;

inline float deg_to_radians(float deg)
{
	return deg * (pi / 180);
}

//unsigned int VBO;
//unsigned int VAO;
//unsigned int IBO;

int model_location;
int projection_location;
int view_location;
int texture_location;

//struct Vertex
//{
//	Vertex() = default;
//	Vertex(glm::vec3 position, glm::vec2 uv) : pos(position), uv(uv)
//	{
//	}
//
//	glm::vec3 pos;
//	glm::vec2 uv;
//};
////
//void create_buffers()
//{
//	glGenVertexArrays(1, &VAO);
//	glBindVertexArray(VAO);
//
//	glm::vec2 t00(0.0f, 0.0f); // bottom left
//	glm::vec2 t01(0.0f, 1.0f); // bottom right
//	glm::vec2 t10(1.0f, 0.0f);  // uper left
//	glm::vec2 t11(1.0f, 1.0f); // upper right
//
//	Vertex vertices[8]
//	{
//		Vertex(glm::vec3(0.5f,  0.5f,  0.5f), t00),
//		Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), t01),
//		Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), t10),
//		Vertex(glm::vec3(0.5f, -0.5f, -0.5f), t11),
//		Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), t00),
//		Vertex(glm::vec3(0.5f,  0.5f, -0.5f), t10),
//		Vertex(glm::vec3(0.5f, -0.5f,  0.5f), t01),
//		Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), t11)
//	};
//
//	glGenBuffers(1, &VBO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
//
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (3 * sizeof(float)));
//
//	unsigned int indices[] = {
//			   0, 1, 2,
//				1, 3, 4,
//				5, 6, 3,
//				7, 3, 6,
//				2, 4, 7,
//				0, 7, 6,
//				0, 5, 1,
//				1, 5, 3,
//				5, 0, 6,
//				7, 4, 3,
//				2, 1, 4,
//				0, 2, 7
//	};
//
//	glGenBuffers(1, &IBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//}

bool read_file(const char* filename, std::string& target)
{
	std::ifstream f(filename);

	if(f.is_open())
	{
		std::string line;
		while(getline(f, line))
		{
			target.append(line);
			target.append("\n");
		}
		f.close();
		return true;
	}

	std::cerr << "ERROR: could not read " << filename << "\n";
	return false;
}

const char* vs_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\v_shader.glsl";
const char* fs_filename = "C:\\croc\\Anubis\\Anubis\\assets\\shaders\\f_shader.glsl";

void add_shader(unsigned int shader_program, const char* shader_text, GLenum shader_type)
{
	unsigned int shader_obj = glCreateShader(shader_type);
	if(shader_obj == 0)
	{
		std::cerr << "error creating shader type " << shader_type << "\n";
		return;
	}

	const char* shader_source[1];
	shader_source[0] = shader_text;

	int shader_length[1];
	shader_length[0] = strlen(shader_text);

	glShaderSource(shader_obj, 1, shader_source, shader_length);
	glCompileShader(shader_obj);

	int success;
	glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		char info_log[1024];
		glGetShaderInfoLog(shader_obj, 1024, NULL, info_log);
		std::cerr << "error compiling shader " << shader_type << " : " << info_log << "\n";
		return;
	}

	glAttachShader(shader_program, shader_obj);
}

void compile_shaders()
{
	unsigned int shader_program = glCreateProgram();
	if(shader_program == 0)
	{
		std::cerr << "error creating shader program \n";
	} 
	
	std::string vs;
	std::string fs;

	if(!read_file(vs_filename, vs))
	{
		std::cerr << "errror reading vertex shader";
	}

	if (!read_file(fs_filename, fs))
	{
		std::cerr << "errror reading vertex shader";
	}

	add_shader(shader_program, vs.c_str(), GL_VERTEX_SHADER);
	add_shader(shader_program, fs.c_str(), GL_FRAGMENT_SHADER);

	int success;
	char info_log[1024];

	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
		std::cerr << "error compiling shader program: " << info_log << "\n";
		return;
	}

	model_location = glGetUniformLocation(shader_program, "model");
	view_location = glGetUniformLocation(shader_program, "view");
	projection_location = glGetUniformLocation(shader_program, "projection");
	texture_location = glGetUniformLocation(shader_program, "texture_sampler");

	glValidateProgram(shader_program);
	glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
		std::cerr << "IVALID shader program: " << info_log << "\n";
		return;
	}


	glUseProgram(shader_program);
}

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


	//create_buffers();
	compile_shaders();

	float scale = 0.0f;
	float delta = 0.0065f;
	float angle = 0.0f;
	float angle_delta = 1.0f;

	glm::mat4 identity(1.0f);

	//glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);


	glm::mat4 projection = glm::perspective(deg_to_radians(90.0f), (float)width / (float)height, .1f, 100.0f);

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
	if(!mesh->load("C:\\croc\\Anubis\\Anubis\\assets\\Content\\spider.obj"))
	{
		return 0;
	}

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
		glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

		scale += delta;
		if(std::abs(scale) >= 1.0f)
		{
			//delta *= -1.0f;
		}

		mesh->get_transform().set_position(0.0f, 0.0f, -1.0f);
		cube_transform.set_position(0.0f, 0.0f, -1.0f);

		angle += angle_delta;
		if(std::abs(deg_to_radians(angle)) >= deg_to_radians(90.0f))
		{
			//angle_delta *= -1.0f;
		}

		mesh->get_transform().set_rotation(0.0f, angle, 0.0f);
		cube_transform.set_rotation(0.0f, angle, 0.0f);

		mesh->get_transform().set_scale(0.3f);
		cube_transform.set_scale(0.01f);
		camera.update(delta_time);

		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(mesh->get_transform().get_matrix()));
		glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(camera.get_look_at()));
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1i(texture_location, 0);
		//glBindVertexArray(VAO);

		mesh->render();
		//texture.use();
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	//glDeleteVertexArrays(1, &VAO);
	//VAO = -1;
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &IBO);
	glfwTerminate();
	return 0;
}