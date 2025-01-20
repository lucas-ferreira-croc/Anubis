#include <iostream>
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