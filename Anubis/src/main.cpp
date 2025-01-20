#include <iostream>
#include <fstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

unsigned int VBO;

void create_vertex_buffer()
{
	glEnable(GL_CULL_FACE);
	glm::vec3 vertices[3] = {
		{-1.0f, -1.0f, 0.0f},
		{ 1.0f, -1.0f, 1.0f},
		{ 0.0f,  1.0f, 0.0f}
	};

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

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
	
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	create_vertex_buffer();
	compile_shaders();

	while(!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}