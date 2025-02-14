#include "PickingTexture.h"

PickingTexture::~PickingTexture()
{
}

void PickingTexture::init(unsigned int window_width, unsigned int window_height)
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_pickingTexture);
	glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, window_width, window_height, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingTexture, 0);

	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_width, window_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "FB ERROR, STATUS: " << status << "\n";
		return;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PickingTexture::enable_writing()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void PickingTexture::disable_writing()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

PickingTexture::PixelInfo PickingTexture::read_pixel(unsigned int x, unsigned int y)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	PixelInfo pixel;
	glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &pixel);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	return pixel;
}
