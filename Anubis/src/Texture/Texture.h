#pragma once

#include <GL/glew.h>

class Texture
{
public:
	Texture();
	Texture(const char* filepath);
	~Texture();

	bool load_texture();
	bool load_textureA();
	void use();
	void clear_texture();

	const char* get_filepath() { return filepath; };
	
	static unsigned int id_counter;
	unsigned int gl_id;
private:
	int width, height, bit_depth;

	const char* filepath;
};

