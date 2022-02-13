#include "Common.h"

//Project includes
#include "Shader.h"
#include "Control.h"
#include "Model.h"
#include "SimpleMath.h"

GLFWwindow* window;

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	void *userParam);

void renderQuad();
void loopPrint(std::string message, float p1, float p2 = NULL, float p3 = NULL);

bool initGLFW();
bool initWindow();
bool initGlew();

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct ModelInstance {

	ModelInstance(Model *model) {
		this->model = model;
		pos = model->getTranslate();
		rot = model->getRotate();
		scale = model->getScale();
	}

	Model *model;
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;

	bool tessellate = false;

	float distanceZ;
};

struct LessThanDistance
{
	inline bool operator() (const ModelInstance& struct1, const ModelInstance& struct2)
	{
		return (struct1.distanceZ < struct2.distanceZ);
	}
};

int main(int argc, char* args[]) {

	// Initialize GLFW
	initGLFW();

	// Window
	initWindow();

	// Initialize GLEW
	initGlew();

	std::cout << glGetString(GL_VERSION) << std::endl;

	// Initiate controls and GLFWwindow
	Control control;
	control.initControl();

	// Debugging output
	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		std::cout << "Debug enable" << std::endl;
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE); //Don't want any output at the moment (it spams without clear)
		//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}


	// configure global opengl state
	// -----------------------------

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glDisable(GL_CULL_FACE);

	// build and compile shaders
	// -------------------------
	Shader shaderGeometryPass("Shader//GPVertex.glsl", "Shader//GPFragment.glsl", "Shader//GPGeometry.glsl", "Shader//GPControl.glsl", "Shader//GPEvaluation.glsl");
	Shader shaderDeferredPass("Shader//DeferredVertex.glsl", "Shader//DeferredFragment.glsl");
	Shader shaderLightCube("Shader//LightVertex.glsl", "Shader//LightFragment.glsl");
	Shader shaderSimpleDepth("Shader//DepthVertex.glsl", "Shader//DepthFragment.glsl");
	
	// ................................................
	// ------------------ MODEL LOAD ------------------
	// ................................................

	//	1.Declare and define a Model object
	//  2.Push the object back in modelArray

	Model groundPlane("Models//Geometry//plane.obj");
	groundPlane.setDiffuse(glm::vec3(0.2, 0.2, 0.2));
	groundPlane.setSpecular(glm::vec3(0.2, 0.2, 0.2));
	groundPlane.setScale(glm::vec3(30, 1, 10));
	groundPlane.setTranslate(glm::vec3(10, 0, 0));

	Model donutObjParse("Models//Geometry//Donut.obj");;
	//donutObjParse.loadModelObjParse("Models//Geometry//Donut.obj");
	donutObjParse.setTranslate(glm::vec3(18, 0.5, 2));
	donutObjParse.setScale(glm::vec3(1, 1, 1));
	donutObjParse.setDiffuse(glm::vec3(0.0f, 1.0f, 0.0f));

	Model coneObjParse("Models//Geometry//Cone.obj");
	//coneObjParse.loadModelObjParse("Models//Geometry//Cone.obj");
	coneObjParse.setTranslate(glm::vec3(22, 1.3, 2));
	coneObjParse.setScale(glm::vec3(1.3, 1.3, 1.3));
	coneObjParse.setDiffuse(glm::vec3(1.0f, 0.0f, 0.0f));

	Model cubeObjParse("Models//Geometry//Cube.obj");
	//cubeObjParse.loadModelObjParse("Models//Geometry//Cube.obj");
	cubeObjParse.setTranslate(glm::vec3(22, 0.8, -2));
	cubeObjParse.setScale(glm::vec3(1.7, 1.7, 1.7));
	cubeObjParse.setDiffuse(glm::vec3(0.0f, 0.0f, 1.0f));

	Model cylinderObjParse("Models//Geometry//Cylinder.obj");
	//cylinderObjParse.loadModelObjParse("Models//Geometry//Cylinder.obj");
	cylinderObjParse.setTranslate(glm::vec3(18, 1, -2));
	cylinderObjParse.setScale(glm::vec3(1, 1, 1));
	cylinderObjParse.setDiffuse(glm::vec3(1.0f, 0.0f, 1.0f));

	Model boat("Models//Boat//boat.obj");
	boat.setScale(glm::vec3(1.0f));
	boat.setRotate(glm::vec3(0.0f, 180.f, 0.0f));

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

	Model lightCube("Models//Geometry//cube.obj");
	lightCube.setDrawMode(GL_TRIANGLES);
	lightCube.setScale(glm::vec3(0.125f));

	Model tessellationPlane("Models//Geometry//plane.obj");
	tessellationPlane.loadMaterial("Aheightmap.png", "height");
	tessellationPlane.setDiffuse(glm::vec3(0.4, 0.4, 0.4));
	tessellationPlane.setSpecular(glm::vec3(0.2, 0.2, 0.2));
	tessellationPlane.setScale(glm::vec3(8, 1, 8));
	tessellationPlane.setTranslate(glm::vec3(10, 0.05, 0));
	tessellationPlane.setDrawMode(GL_TRIANGLES);

	// Store all models in array
	std::vector<ModelInstance> modelArray;

	ModelInstance groundPlaneIn(&groundPlane);
	modelArray.push_back(groundPlaneIn);

	ModelInstance donutObjParseIn(&donutObjParse);
	modelArray.push_back(donutObjParseIn);

	ModelInstance coneObjParseIn(&coneObjParse);
	modelArray.push_back(coneObjParseIn);

	ModelInstance cubeObjParseIn(&cubeObjParse);
	modelArray.push_back(cubeObjParseIn);

	ModelInstance cylinderObjParseIn(&cylinderObjParse);
	modelArray.push_back(cylinderObjParseIn);

	for (int i = 0; i < objectPositions.size(); i++)
	{
		ModelInstance boatIn(&boat);
		boatIn.pos = objectPositions[i];
		boatIn.scale = glm::vec3(0.25f);
		modelArray.push_back(boatIn);
	}

	ModelInstance tessellationPlaneIn(&tessellationPlane);
	tessellationPlaneIn.tessellate = true;
	modelArray.push_back(tessellationPlaneIn);

	
	// ................................................
	// ------------------- BUFFERS --------------------
	// ................................................


	// configure depthMap(shadowMap) frambuffer
	// ----------------------------------------
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// finally check if framebuffer is completeglBindTexture
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// configure g-buffer framebuffer
	// ------------------------------
	unsigned int gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int gPosition, gNormal, gDiffuse, gSpecular, gShadow, gTangent, gBiTangent, gFaceNormal;
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
	// diffuse color buffer
	glGenTextures(1, &gDiffuse);
	glBindTexture(GL_TEXTURE_2D, gDiffuse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gDiffuse, 0);
	// specular color buffer
	glGenTextures(1, &gSpecular);
	glBindTexture(GL_TEXTURE_2D, gSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gSpecular, 0);
	// shadow color buffer
	glGenTextures(1, &gShadow);
	glBindTexture(GL_TEXTURE_2D, gShadow);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gShadow, 0);
	// tangent buffer
	glGenTextures(1, &gTangent);
	glBindTexture(GL_TEXTURE_2D, gTangent);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gTangent, 0);
	// bitangent buffer
	glGenTextures(1, &gBiTangent);
	glBindTexture(GL_TEXTURE_2D, gBiTangent);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, gBiTangent, 0);
	// bitangent buffer
	glGenTextures(1, &gFaceNormal);
	glBindTexture(GL_TEXTURE_2D, gFaceNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, GL_TEXTURE_2D, gFaceNormal, 0);

	//sampler2D diffuse;
	//sampler2D specular;
	//sampler2D normal;

	// create and attach depth buffer (framebuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[8] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
	glDrawBuffers(8, attachments);
	
	// finally check if framebuffer is completeglBindTexture
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ................................................
	// ------------------- LIGHTS ---------------------
	// ................................................

	// Directional Light (with shadows)

	float near_plane = 1.0f, far_plane = 50.0f;

	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

	// lighting info
	// -------------
	const unsigned int NR_LIGHTS = 64;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightPositionsOrg;
	std::vector<glm::vec3> lightDiffuse;
	std::vector<glm::vec3> lightSpecular;
	srand(0);

	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0) * 30 - 4.5;
		float yPos = ((rand() % 100) / 100.0) * 0.6 - 0.0;
		float zPos = ((rand() % 100) / 100.0) * 9.0 - 4.5;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0

		lightDiffuse.push_back(glm::vec3(rColor, gColor, bColor));
		lightSpecular.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	}
	lightPositionsOrg = lightPositions;
	
	// --------------------
	// shader configuration
	// --------------------

	shaderGeometryPass.useProgram();
	shaderGeometryPass.setInt("shadowMap", 4);


	shaderDeferredPass.useProgram();
	shaderDeferredPass.setInt("gPosition", 0);
	shaderDeferredPass.setInt("gNormal", 1);
	shaderDeferredPass.setInt("gDiffuse", 2);
	shaderDeferredPass.setInt("gSpecular", 3);
	shaderDeferredPass.setInt("gShadow", 4);
	shaderDeferredPass.setInt("gTangent", 5);
	shaderDeferredPass.setInt("gBiTangent", 6);
	shaderDeferredPass.setInt("gFaceNormal", 7);

	// ................................................
	// ----------------- RENDER LOOP ------------------
	// ................................................

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) { 

		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		control.computeMatricesFromInputs();

		//loopPrint("Camera position", control.getPosition().x, control.getPosition().y, control.getPosition().z);

		// Calculate Directional Light with shadows
		glm::vec3 dirLightPosition = glm::vec3(-3.0f, 5.0f, 2.0f);
		glm::vec3 dirLightDirection = glm::vec3(0.0f, 0.0f, 0.0f) - dirLightPosition;

		glm::mat4 lightView = glm::lookAt(
			dirLightPosition,						//Position
			dirLightPosition + dirLightDirection,	//Position + Direction
			glm::vec3(0.0f, 1.0f, 0.0f));			//Up

		glm::vec3 dirLightDiffuse = glm::vec3(0.7f, 0.7f, 0.7f);
		glm::vec3 dirLightSpecular = glm::vec3(0.5f, 0.5f, 0.5f);

		// render
		// ------

		// 1. first render to depth map
		shaderSimpleDepth.useProgram();
		shaderSimpleDepth.setMat4("lightView", lightView);
		shaderSimpleDepth.setMat4("lightProjection", lightProjection);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glCullFace(GL_FRONT);
		// vertex shader buffer
		// draw objects in scene and save depth buffer (shadow depth) from light position

		// RenderScene() - Draw Geometry
		for (int i = 0; i < modelArray.size(); i++)
		{
			//SHADER simpleShader
			glm::mat4 ModelMatrix = modelArray[i].model->getModel();

			shaderSimpleDepth.setMat4("model", ModelMatrix);

			modelArray[i].model->setDrawMode(GL_TRIANGLES);

			modelArray[i].model->setTranslate(modelArray[i].pos);
			modelArray[i].model->setRotate(modelArray[i].rot);
			modelArray[i].model->setScale(modelArray[i].scale);

			modelArray[i].model->Draw(shaderSimpleDepth);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 2. then render scene as normal with shadow mapping (using depth map)

		// 1. geometry pass: render scene's geometry/color data into gbuffer
		// -----------------------------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer); //Start sending objects to gBuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		
		for (int i = 0; i < modelArray.size(); i++)
		{
			float dist;
			dist = glm::distance(control.getPosition(), modelArray[i].pos);
			modelArray[i].distanceZ = dist;
		}

		//Front to Back
		std::sort(modelArray.begin(), modelArray.end(), LessThanDistance());

		//Just to show that shadow mapping updates
		//modelArray[0].rot = glm::vec3(0, linearGrowth(0, 5.0f), 0);


		shaderGeometryPass.useProgram();

		// RenderScene() - Draw Geometry
		for (int i = 0; i < modelArray.size(); i++)
		{
			//SHADER shaderGeometryPass
			glm::mat4 ModelMatrix = modelArray[i].model->getModel();
			glm::mat4 ViewMatrix = control.getViewMatrix();
			glm::mat4 ProjectionMatrix = control.getProjectionMatrix();

			shaderGeometryPass.setMat4("model", ModelMatrix);
			shaderGeometryPass.setMat4("view", ViewMatrix);
			shaderGeometryPass.setMat4("projection", ProjectionMatrix);

			shaderGeometryPass.setVec3("viewPos", control.getPosition());
			shaderGeometryPass.setVec3("gEyeWorldPos", control.getPosition());
			shaderGeometryPass.setMat4("gVP", control.getProjectionMatrix() * control.getViewMatrix());
			shaderGeometryPass.setBool("gTessellate", modelArray[i].tessellate);
			shaderGeometryPass.setFloat("gDispFactor", 1.0f);

			shaderGeometryPass.setMat4("lightView", lightView);
			shaderGeometryPass.setMat4("lightProjection", lightProjection);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, depthMap);

			modelArray[i].model->setDrawMode(GL_PATCHES);

			modelArray[i].model->setTranslate(modelArray[i].pos);
			modelArray[i].model->setRotate(modelArray[i].rot);
			modelArray[i].model->setScale(modelArray[i].scale);

			modelArray[i].model->Draw(shaderGeometryPass);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0); //End sending objects to gBuffer

		// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
		// -----------------------------------------------------------------------------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		shaderDeferredPass.useProgram();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gDiffuse);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gSpecular);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, gShadow);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, gTangent);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, gBiTangent);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, gFaceNormal);

		// send light relevant uniforms

		shaderDeferredPass.setVec3("dirLight.direction", dirLightDirection);
		shaderDeferredPass.setVec3("dirLight.diffuse", dirLightDiffuse);
		shaderDeferredPass.setVec3("dirLight.specular", dirLightSpecular);

		for (unsigned int i = 0; i < NR_LIGHTS; i++)
		{
			//SHADER shaderDeferredPass

			shaderDeferredPass.setVec3("pointLights[" + std::to_string(i) + "].position", lightPositions[i]);

			// light properties
			shaderDeferredPass.setVec3("pointLights[" + std::to_string(i) + "].diffuse", lightDiffuse[i]);
			shaderDeferredPass.setVec3("pointLights[" + std::to_string(i) + "].specular", lightSpecular[i]);

			// update attenuation parameters and calculate radius
			const float constant = 1.0;
			const float linear = std::pow(0.7, 4);
			const float quadratic = std::pow(1.8, 4);
			shaderDeferredPass.setFloat("pointLights[" + std::to_string(i) + "].constant", constant);
			shaderDeferredPass.setFloat("pointLights[" + std::to_string(i) + "].linear", linear);
			shaderDeferredPass.setFloat("pointLights[" + std::to_string(i) + "].quadratic", quadratic);

			// then calculate radius of light volume/sphere
			const float maxBrightness = std::fmaxf(std::fmaxf(lightDiffuse[i].r, lightDiffuse[i].g), lightDiffuse[i].b);
			float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
			shaderDeferredPass.setFloat("pointLights[" + std::to_string(i) + "].radius", radius);
		}

		shaderDeferredPass.setVec3("viewPos", control.getPosition());
		// finally render quad
		renderQuad();

		// 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
		// ----------------------------------------------------------------------------------
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		// blit to default framebuffer.
		glBlitFramebuffer(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. render lights on top of scene
		// --------------------------------

		// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
		// -----------------------------------------------------------------------------------------------------------------------
		shaderLightCube.useProgram();
		shaderLightCube.setMat4("projection", control.getProjectionMatrix());
		shaderLightCube.setMat4("view", control.getViewMatrix());
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			shaderLightCube.setMat4("model" ,lightCube.getModel());
			lightCube.setTranslate(lightPositions[i]);
			shaderLightCube.setVec3("lightColor", lightDiffuse[i]);
			lightCube.Draw(shaderLightCube);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup VBO and shader
	glDeleteBuffers(1, &gBuffer);
	glDeleteBuffers(1, &rboDepth);
	//glDeleteProgram(shader.ProgramID);
	//glDeleteTextures(1, &Texture);
	//glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void loopPrint(std::string message, float p1, float p2, float p3)
{
	std::cout << std::fixed;

	std::stringstream string;
	std::string append;
	std::setprecision(2);
	if (p2 == NULL)
	{
		string << message << ": (" + std::to_string(p1) + ")";
	}

	else if (p3 == NULL)
	{
		string << message << ": (" + std::to_string(p1) + " ," + std::to_string(p2) + ")";
	}

	else
	{
		string << message << ": (" + std::to_string(p1) + " ," + std::to_string(p2) + " ," + std::to_string(p3) + ")";
	}


	for (int i = 0; i < string.str().length(); i++)
		append += " ";
	append += '\r';
	std::cout << string.str() << append;

}

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	void *userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << std::fixed;

	std::stringstream output;
	std::string append;

	output << "---------------" << std::endl;
	output << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             output << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   output << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: output << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     output << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     output << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           output << "Source: Other"; break;
	} output << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               output << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: output << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  output << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         output << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         output << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              output << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          output << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           output << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               output << "Type: Other"; break;
	} output << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         output << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       output << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          output << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: output << "Severity: notification"; break;
	} output << std::endl;
	output << std::endl;



	for (int i = 0; i < output.str().length(); i++)
		append += " ";
	append += '\r';
	std::cout << output.str() << append;
}

bool initGLFW()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW/n");
		getchar();
		return -1;
	}

	return true;
}

bool initWindow() {

	// Window
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL Window", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. Intel GPU may require version 2.1 instead of 3.3.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	return true;
}

bool initGlew() {

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	return true;
}