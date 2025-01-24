#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../vendor/stb_image.h"

#include <iostream>


Texture::Texture()
	: width(0), height(0), bit_depth(0), filepath("")
{
}


Texture::Texture(GLenum textureTarget, const char* filepath)
	:  width(0), height(0), bit_depth(0), filepath(filepath), m_textureTarget(textureTarget)
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

	glGenTextures(1, &m_TextureObject);
	glBindTexture(GL_TEXTURE_2D, m_TextureObject);

	glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(m_textureTarget, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);

	glGenerateMipmap(m_textureTarget);
	glBindTexture(m_textureTarget, 0);

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


	glGenTextures(1, &m_TextureObject);
	glBindTexture(GL_TEXTURE_2D, m_TextureObject);

	glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(m_textureTarget, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);

	glGenerateMipmap(m_textureTarget);
	glBindTexture(m_textureTarget, 0);

	stbi_image_free(tex_data);

	return true;
}

void Texture::use(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(m_textureTarget, m_TextureObject);
}

void Texture::clear_texture()
{
	glDeleteTextures(1, &m_TextureObject);
	m_TextureObject = 0;
	width = 0;
	height = 0;
	bit_depth = 0;
	filepath = (char*)"";
}