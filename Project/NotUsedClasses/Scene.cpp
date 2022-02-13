#include "Scene.h"



Scene::Scene(Control &control)
{
	this->control = &control;
}


Scene::~Scene()
{
	delete control;
}

void Scene::initialize()
{
	// initialize the scene

	shaderSimple.loadShader("Shader//LightVertex.glsl", "Shader//LightFragment.glsl");
	shaderGeometry.loadShader("Shader//GPVertex.glsl", "Shader//GPFragment.glsl", "Shader//GPGeometry.glsl", "Shader//GPControlglsl", "Shader//GPEvaluation.glsl");
	shaderGeometryTess.loadShader("Shader//GPVertex_Tess.glsl", "Shader//GPFragment_Tess.glsl", "Shader//GPGeometry_Tess.glsl", "Shader//GPControl_Tess.glsl", "Shader//GPEvaluation_Tess.glsl");
	shaderDeferred.loadShader("Shader//DeferredVertex.glsl", "Shader//DeferredFragment.glsl");

	loadModels();
	initGBuffer();

	for (int i = 0; i < drawArray.size(); i++)
	{
		drawArray[i].pos = drawArray[i].model->getTranslate();
		drawArray[i].rot = drawArray[i].model->getRotate();
		drawArray[i].scale = drawArray[i].model->getScale();
	}

	// lighting info
	// -------------
	const unsigned int NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightPositionsOrg;
	std::vector<glm::vec3> lightColors;
	srand(13);

	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0) * 9.0 - 4.5;
		float yPos = ((rand() % 100) / 100.0) * 0.5 - 0.0;
		float zPos = ((rand() % 100) / 100.0) * 9.0 - 4.5;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0

		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}
	lightPositionsOrg = lightPositions;

	// shader configuration
	// --------------------
	shaderDeferred.useProgram();
	shaderDeferred.setInt("gPosition", 0);
	shaderDeferred.setInt("gNormal", 1);
	shaderDeferred.setInt("gAlbedo", 2);
	shaderDeferred.setInt("gSpecular", 3);

}

void Scene::renderScene()
{
	// per-frame time logic
	// --------------------
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// input
	// -----
	control->computeMatricesFromInputs();

	// 1. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer); //Start sending objects to gBuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	//SHADER shaderGeometryPass
	shaderGeometry.useProgram();
	shaderGeometry.setVec3("viewPos", control->getPosition());
	shaderGeometry.setVec3("gEyeWorldPos", control->getPosition());
	shaderGeometry.setMat4("gVP", control->getProjectionMatrix() * control->getViewMatrix());



	// renders the scene

	//sortArray(front2back);

	//Either have a transformation function where all the update pos, rot, scale are done in (cleaner)
	//or do the transformation here in renderscene (not as clean)
	//transformation();


	//before drawing one model, need to send in shader variables
	//also need to calculate distance from camera (save in distanceZ)
	drawAll();

}

void Scene::loadModels()
{
	// load models
	// -----------

	std::vector<glm::vec3> planeVertices{
	glm::vec3(-1.0f, 1.0f, 0.0f),
	glm::vec3(-1.0f, -1.0f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(-1.0f, -1.0f, 0.0f),
	glm::vec3(1.0f, -1.0f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f)
	};
	std::vector<glm::vec2> planeTexCoords{
		glm::vec2(0, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1)
	};
	std::vector<glm::vec3> planeNormals{
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0)
	};

	Model groundPlane(planeVertices, planeTexCoords, planeNormals);
	groundPlane.loadShader(shaderGeometry);
	groundPlane.loadProjectionMatrix(control->getProjMtx());
	groundPlane.loadViewMatrix(control->getViewMtx());

	groundPlane.loadMaterial("brickwall_normal.jpg", "normal");
	groundPlane.loadMaterial("AlphaTexture.jpg", "height");
	groundPlane.setDiffuse(glm::vec3(0.2, 0.2, 0.2));

	groundPlane.setRotate(glm::vec3(90, 0, 0));
	groundPlane.setScale(glm::vec3(20, 20, 1));
	groundPlane.setTranslate(glm::vec3(15, 0, 15));


	Model backFaceCullPlane(planeVertices, planeTexCoords, planeNormals);
	backFaceCullPlane.loadShader(shaderGeometry);
	backFaceCullPlane.loadProjectionMatrix(control->getProjMtx());
	backFaceCullPlane.loadViewMatrix(control->getViewMtx());

	backFaceCullPlane.setTranslate(glm::vec3(20, 1, 0));

	//drawArray.push_back(backFaceCullPlane);

	Model boat("Models//boat.obj");
	boat.loadShader(shaderGeometry);
	boat.loadProjectionMatrix(control->getProjMtx());
	boat.loadViewMatrix(control->getViewMtx());

	std::vector<glm::vec3> objectPositions;
	objectPositions.push_back(glm::vec3(-3.0, 0.0, -3.0));
	objectPositions.push_back(glm::vec3(0.0, 0.0, -3.0));
	objectPositions.push_back(glm::vec3(3.0, 0.0, -3.0));
	objectPositions.push_back(glm::vec3(-3.0, 0.0, 0.0));
	objectPositions.push_back(glm::vec3(0.0, 0.0, 0.0));
	objectPositions.push_back(glm::vec3(3.0, 0.0, 0.0));
	objectPositions.push_back(glm::vec3(-3.0, 0.0, 3.0));
	objectPositions.push_back(glm::vec3(0.0, 0.0, 3.0));
	objectPositions.push_back(glm::vec3(3.0, 0.0, 3.0));

	std::vector<glm::vec3> cubeVertices{
	glm::vec3(-0.5f, 0.5f, 0.5f),
	glm::vec3(-0.5f, -0.5f, 0.5f),
	glm::vec3(0.5f, 0.5f, 0.5f),
	glm::vec3(-0.5f, -0.5f, 0.5f),
	glm::vec3(0.5f, -0.5f, 0.5f),
	glm::vec3(0.5f, 0.5f, 0.5f),

	glm::vec3(0.5f, 0.5f, 0.5f),
	glm::vec3(0.5f, -0.5f, 0.5f),
	glm::vec3(0.5f, 0.5f, -0.5f),
	glm::vec3(0.5f, -0.5f, 0.5f),
	glm::vec3(0.5f, -0.5f, -0.5),
	glm::vec3(0.5f, 0.5f, -0.5f),

	glm::vec3(0.5f, 0.5f, -0.5f),
	glm::vec3(0.5f, -0.5f, -0.5f),
	glm::vec3(-0.5f, 0.5f, -0.5f),
	glm::vec3(0.5f, -0.5f, -0.5f),
	glm::vec3(-0.5f, -0.5f, -0.5f),
	glm::vec3(-0.5f, 0.5f, -0.5f),

	glm::vec3(-0.5f, 0.5f, -0.5f),
	glm::vec3(-0.5f, -0.5f, -0.5f),
	glm::vec3(-0.5f, 0.5f, 0.5f),
	glm::vec3(-0.5f, -0.5f, -0.5f),
	glm::vec3(-0.5f, -0.5f, 0.5f),
	glm::vec3(-0.5f, 0.5f, 0.5f),

	glm::vec3(-0.5f, 0.5f, -0.5f),
	glm::vec3(-0.5f, 0.5f, 0.5f),
	glm::vec3(0.5f, 0.5f, -0.5f),
	glm::vec3(-0.5f, 0.5f, 0.5f),
	glm::vec3(0.5f, 0.5f, 0.5f),
	glm::vec3(0.5f, 0.5f, -0.5f),

	glm::vec3(-0.5f, -0.5f, 0.5f),
	glm::vec3(-0.5f, -0.5f, -0.5f),
	glm::vec3(0.5f, -0.5f, 0.5f),
	glm::vec3(-0.5f, -0.5f, -0.5f),
	glm::vec3(0.5f, -0.5f, -0.5f),
	glm::vec3(0.5f, -0.5f, 0.5f)
	};
	std::vector<glm::vec2> cubeTexCoords{
		glm::vec2(0, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),

		glm::vec2(0, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),

		glm::vec2(0, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),

		glm::vec2(0, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),

		glm::vec2(0, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),

		glm::vec2(0, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
	};
	std::vector<glm::vec3> cubeNormals{
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),

		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),

		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),

		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),

		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),

		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0)
	};

	Model lightCube(cubeVertices, cubeTexCoords, cubeNormals);
	lightCube.loadShader(shaderSimple);
	lightCube.loadProjectionMatrix(control->getProjMtx());
	lightCube.loadViewMatrix(control->getViewMtx());
	lightCube.setDrawMode(GL_TRIANGLES);


	Model tessellationPlane(planeVertices, planeTexCoords, planeNormals);
	tessellationPlane.loadShader(shaderGeometryTess);
	tessellationPlane.loadProjectionMatrix(control->getProjMtx());
	tessellationPlane.loadViewMatrix(control->getViewMtx());

	tessellationPlane.loadMaterial("brickwall_normal.jpg", "normal");
	tessellationPlane.loadMaterial("AlphaTexture.jpg", "height");
	tessellationPlane.setDiffuse(glm::vec3(0.2, 0.2, 0.2));

	tessellationPlane.setRotate(glm::vec3(90, 0, 0));
	tessellationPlane.setScale(glm::vec3(5, 5, 1));
	tessellationPlane.setTranslate(glm::vec3(10, 0, 0));


	ModelInstance modelInstance;
	modelInstance.model = &groundPlane;
	modelInstance.pos = groundPlane.getTranslate();
	modelInstance.rot = groundPlane.getRotate();
	modelInstance.scale = groundPlane.getScale();
	modelInstance.passive = true;
}

void Scene::initGBuffer()
{
	// configure g-buffer framebuffer
	// ------------------------------
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	// position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// albedo color buffer
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
	// specular color buffer
	glGenTextures(1, &gSpecular);
	glBindTexture(GL_TEXTURE_2D, gSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gSpecular, 0);

	//sampler2D diffuse;
	//sampler2D specular;
	//sampler2D normal;

	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);


	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::drawAll()
{
	for (int i = 0; i < drawArray.size(); i++)
	{

		if (!drawArray[i].passive)
		{
			drawArray[i].model->setTranslate(drawArray[i].pos);
			drawArray[i].model->setRotate(drawArray[i].rot);
			drawArray[i].model->setScale(drawArray[i].scale);
		}
		//lets say the useProgram also sends in variable data to shader and is used when model.draw();
		drawArray[i].model->Draw(shaderGeometry);
	}
}
