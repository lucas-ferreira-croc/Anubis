#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../vendor/stb_image.h"

#include <iostream>


Texture::Texture()
	: width(0), height(0), bit_depth(0), filepath(""), gl_id(0)
{
}


Texture::Texture(const char* filepath)
	:  width(0), height(0), bit_depth(0), filepath(filepath), gl_id(0)
{
}

Texture::~Texture()
{
	clear_texture();
}

bool Texture::load_texture()
{
	stbi_set_flip_vertically_on_load(true);

	unsigned char* tex_data = stbi_load(filepath, &width, &height, &bit_depth, 0);
	if (!tex_data)
	{
		std::cout << "Could not find: " << filepath << "\n";
		return false;
	}

	glGenTextures(1, &gl_id);
	glBindTexture(GL_TEXTURE_2D, gl_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(tex_data);

	return true;
}

bool Texture::load_textureA()
{
	stbi_set_flip_vertically_on_load(true);

	int force_channels = 4;
	unsigned char* tex_data = stbi_load(filepath, &width, &height, &bit_depth, force_channels);
	if (!tex_data)
	{
		std::cout << "Could not find: " << filepath << "\n";
		return false;
	}


	glGenTextures(1, &gl_id);
	glBindTexture(GL_TEXTURE_2D, gl_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(tex_data);

	return true;
}

void Texture::use()
{
	int bound_texture = GL_TEXTURE0;
	if (gl_id != 0)
		bound_texture += gl_id;
	glActiveTexture(bound_texture);
	glBindTexture(GL_TEXTURE_2D, gl_id);
}

void Texture::clear_texture()
{
	glDeleteTextures(1, &gl_id);
	gl_id = 0;
	width = 0;
	height = 0;
	bit_depth = 0;
	filepath = (char*)"";
}