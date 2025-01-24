#pragma once

#include <GL/glew.h>

class Texture
{
public:
	Texture();
	Texture(GLenum textureTarget, const char* filepath);
	~Texture();

	bool load_texture();
	bool load_textureA();
	void use(GLenum textureUnit);
	void clear_texture();

	const char* get_filepath() { return filepath; };
	
	GLenum m_textureTarget;
	unsigned int m_TextureObject;
private:
	int width, height, bit_depth;

	const char* filepath;
};

