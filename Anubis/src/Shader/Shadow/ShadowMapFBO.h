#pragma once

#include <GL/glew.h>

class ShadowMapFBO
{
public:
	ShadowMapFBO();
	~ShadowMapFBO();

	bool init(unsigned int width, unsigned int height);
	
	void bind_for_writing();
	void bind_for_reading(GLenum texture_unit);
private:

	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_fbo;
	unsigned int m_shadowMap;


};

