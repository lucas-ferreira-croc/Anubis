#include "Display.h"
#include <iostream>
#include <vector>

Display::Display(unsigned int window_width, unsigned int window_height)
	: window_width(window_width), window_height(window_height),
	window(nullptr)
{
}

Display::~Display()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Display::initialize_window()
{
	if (!glfwInit())
	{
		std::cout << "GLFW Failed to init!" << std::endl;
		glfwTerminate();

		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(window_width, window_height, "Anubis", NULL, NULL);

	if (!window)
	{
		std::cout << "GLFW failed to create window!" << std::endl;
		glfwTerminate();
		return;
	}

	glfwGetFramebufferSize(window, &buffer_width, &buffer_height);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	create_callbacks();

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "glew init failed" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();

		return;
	}

	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glViewport(0, 0, buffer_width, buffer_height);
	glfwSetWindowUserPointer(window, this);
}

void Display::swap_buffers()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Display::clear_color(int r, int g, int b, int a)
{
	glClearColor(r, g, b, a);
}

void Display::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Display::should_close()
{
	return glfwWindowShouldClose(window);
}

void Display::create_callbacks()
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void Display::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Display* display = static_cast<Display*>(glfwGetWindowUserPointer(window));
	glViewport(0, 0, width, height);

	display->window_width = width;
	display->window_height = height;
}


