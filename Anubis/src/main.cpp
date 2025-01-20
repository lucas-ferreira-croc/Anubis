#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

	while(!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}