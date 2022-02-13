#pragma once

#include <iostream>
#include <GL\glew.h>
#include <GLM\glm.hpp>

class DeferredShading
{
public:
	DeferredShading();
	~DeferredShading();

	unsigned int getGbuffer();
private:

	unsigned int gPosition, gNormal, gAlbedoSpec;

	int SCR_WIDTH;
	int SCR_HEIGHT;

	unsigned int rboDepth;

	unsigned int gBuffer;
};

