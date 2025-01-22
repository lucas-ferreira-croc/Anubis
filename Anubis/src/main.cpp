#include <iostream>
#include <fstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


const double pi = 3.14159265358979323846;

inline float deg_to_radians(float deg)
{
	return deg * (pi / 180);
}

unsigned int VBO;
unsigned int IBO;

int model_location;
int projection_location;

struct Vertex
{
	Vertex() = default;
	Vertex(float x, float y, float z) : pos(x, y, z) 
	{
		float red   = (float)rand() / (float)RAND_MAX;
		float green = (float)rand() / (float)RAND_MAX;
		float blue  = (float)rand() / (float)RAND_MAX;
		
		color = glm::vec3(red, green, blue);
	}

	//Vertex(float x, float y, float z) : pos(x, y, z), color(1.0f, 1.0f, 1.0f) {}

	glm::vec3 pos;
	glm::vec3 color;
};

void create_buffers()
{
	glEnable(GL_CULL_FACE);
	Vertex vertices[8] = {
		Vertex(0.5f,  0.5f,  0.5f), // Vértice 0
		Vertex(0.5f, -0.5f,  0.5f), // Vértice 1
		Vertex(-0.5f, -0.5f,  0.5f), // Vértice 2
		Vertex(-0.5f,  0.5f,  0.5f), // Vértice 3
		Vertex(0.5f,  0.5f, -0.5f), // Vértice 4
		Vertex(0.5f, -0.5f, -0.5f), // Vértice 5
		Vertex(-0.5f, -0.5f, -0.5f), // Vértice 6
		Vertex(-0.5f,  0.5f, -0.5f)  // Vértice 7
	};


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));

	unsigned int indices[] = {
		// Frente
		0, 1, 2,
		2, 3, 0,
		// Trás
		4, 7, 6,
		6, 5, 4,
		// Esquerda
		3, 2, 6,
		6, 7, 3,
		// Direita
		0, 4, 5,
		5, 1, 0,
		// Topo
		0, 3, 7,
		7, 4, 0,
		// Base
		1, 5, 6,
		6, 2, 1
	};


	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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

	model_location = glGetUniformLocation(shader_program, "model");
	projection_location = glGetUniformLocation(shader_program, "projection");
	
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

	create_buffers();
	compile_shaders();

	float scale = 0.0f;
	float delta = 0.0065f;
	float angle = 0.0f;
	float angle_delta = 1.0f;

	glm::mat4 identity(1.0f);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	glm::mat4 projection = glm::perspective(deg_to_radians(90.0f), (float)width / (float)height, .1f, 100.0f);
	glm::mat4 fake_projcection(1.0f);

	while(!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

		scale += delta;
		if(std::abs(scale) >= 1.0f)
		{
			//delta *= -1.0f;
		}
		//glm::mat4 translation = glm::translate(identity, glm::vec3(0.5f, 0.0f, 0.0f));
		glm::mat4 translation = glm::translate(identity, glm::vec3(0.0f , 0.0f, -1.0f));


		angle += angle_delta;
		if(std::abs(deg_to_radians(angle)) >= deg_to_radians(90.0f))
		{
			//angle_delta *= -1.0f;
		}

		glm::mat4 rotation = glm::rotate(identity, deg_to_radians(angle), glm::vec3(0.0f, 1.0f, .0f));
		
		glm::mat4 scale = glm::scale(identity, glm::vec3(0.3f, 0.3f, 0.3f));

		
		glm::mat4 model = translation * rotation * scale;
		//glm::mat4 model = scale * rotation  * translation;
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}