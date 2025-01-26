#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>

class Display
{
public:
	Display(unsigned int window_width, unsigned int window_height);
	~Display();

	void initialize_window();

	void swap_buffers();

	bool get_is_running() { return is_running; };

	void clear_color(int r, int g, int b, int a);
	void clear();

	bool should_close();

	GLFWwindow* get_window() { return window; };

	int get_buffer_width() { return buffer_width; };
	int get_buffer_height() { return buffer_height; };

private:
	GLFWwindow* window;

	unsigned int window_width;
	unsigned int window_height;

	int buffer_width;
	int buffer_height;

	bool is_running;

	unsigned int grid_vbo;
	unsigned int grid_vao;

private:

	void create_callbacks();

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};

