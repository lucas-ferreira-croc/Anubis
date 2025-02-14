#pragma once

#include <iostream>

#include <GL/glew.h>

class PickingTexture
{
public:
	PickingTexture() {}
	~PickingTexture();

	void init(unsigned int window_width, unsigned int window_height);
	
	void enable_writing();
	void disable_writing();

	struct PixelInfo
	{
		unsigned int ObjectID = 0;
		unsigned int DrawID = 0;
		unsigned int PrimID = 0;

		void print()
		{
			std::cout << "Object " << ObjectID << " draw " << DrawID << " prim " << PrimID << "\n";
		}
	};

	PixelInfo read_pixel(unsigned int x, unsigned int y);

private:
	unsigned int m_fbo = 0;
	unsigned int m_pickingTexture = 0;
	unsigned int m_depthTexture = 0;
};

