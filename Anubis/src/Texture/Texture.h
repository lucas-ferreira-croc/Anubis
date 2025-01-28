#pragma once

#include <GL/glew.h>
#include <string>

class Texture
{
public:
	Texture();
	Texture(GLenum textureTarget, std::string filepath);
	~Texture();

	bool load_texture();
	bool load_textureA();
	bool load_texture_grayscale();
	void use(GLenum textureUnit);
	void clear_texture();

	std::string get_filepath() { return filepath; };
	
	GLenum m_textureTarget;
	unsigned int m_TextureObject;
private:
	int width, height, bit_depth;

	std::string filepath;
};

