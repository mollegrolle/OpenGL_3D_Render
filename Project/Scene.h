#pragma once

#include "Common.h"

#include "Control.h"
#include "Shader.h"
#include "Model.h"

struct ModelInstance {
	Model *model;

	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;

	float distanceZ;

	bool passive = true;
};

class Scene
{
public:
	Scene(Control &control);
	~Scene();

	void initialize();

	void renderScene();

private:

	Control *control;

	Shader shaderSimple;
	Shader shaderGeometry;
	Shader shaderGeometryTess;
	Shader shaderDeferred;

	void loadModels();

	std::vector<ModelInstance> drawArray;

	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedo, gSpecular;
	unsigned int rboDepth;

	void initGBuffer();

	void drawAll();
	void drawSpecific(int index);
};

