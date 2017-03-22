#include <GL\glew.h>
#include <GL\GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <SFML\Window.hpp>
#include <SFML\OpenGL.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include <AntTweakBar.h>
#include <SOIL.h>
#include "Model.h"
#include "Camera.h"
#include "Shader.h"
#include "Cannon.h"
#include "Terrain.h"
#pragma comment(lib, "opengl32.lib")
//Initial resolutions
const int RESOLUTION_WIDTH = sf::VideoMode::getDesktopMode().width;
const int RESOLUTION_HEIGHT = sf::VideoMode::getDesktopMode().height;
const int windowWidth = 1280;
const int windowHeight = 720;
bool debug = true;


//Terrain
Terrain *terrain;
//Camera
Camera playerCamera;
//gBuffer
GLuint gBuffer;
//gBuffer Shaders
Shader shaderGeometryPass;
Shader shaderLightningPass;
Shader depthShader;
//gBuffer Textures
GLuint gPosition, gNormal, gAlbedoSpec, gAmbient;
//Quad VAO and VBO
GLuint quadVAO = 0;
GLuint quadVBO;

//Temp values for textures and all the lights
GLuint diffuseTexture;
GLuint specularTexture;
GLuint normalMap;
const GLuint NR_LIGHTS = 32;
std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;
//Stuff for ShadowMap
const GLuint SHADOW_WIDTH = 1024*4, SHADOW_HEIGHT = 1024*4;
GLuint depthMapFBO;
GLuint depthMap;

glm::mat4 lightProjection;
glm::mat4 lightView;
glm::mat4 lightSpaceMatrix;
GLfloat near_plane = 1.0f, far_plane = 7.5f;

//Timing control for controls and camera
sf::Clock deltaClock;
sf::Time deltaTime;
//Matrices
glm::mat4 viewMatrix = glm::lookAt(
	glm::vec3(0, 0, 2),
	glm::vec3(0, 0, 0),
	glm::vec3(0, 1, 0));
glm::mat4 projectionMatrix = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 0.1f, 20.0f);
//All models in the program
std::vector<Model*> allModels;
std::vector<Model> modelLibrary;
//AntTweakBar
TwBar *debugInterface;
int amountOfHits;
int amountOfTriesLeft;
//Cannon
Cannon aCannon;

void cleanup() 
{
	delete terrain;
}

void renderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

}

void createShadowMap()
{
	depthShader = Shader("shadowVertex.glsl", "shadowFragment.glsl");

	// Configure depth map FBO
	glGenFramebuffers(1, &depthMapFBO);
	// - Create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void createGBuffer()
{
	shaderGeometryPass = Shader("gBufferGeometryVertex.glsl", "gBufferGeometryFragment.glsl");
	shaderLightningPass = Shader("gBufferLightningVertex.glsl", "gBufferLightningFragment.glsl");

	// Set samplers
	shaderLightningPass.use();
	glUniform1i(glGetUniformLocation(shaderLightningPass.program, "gPosition"), 0);
	glUniform1i(glGetUniformLocation(shaderLightningPass.program, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(shaderLightningPass.program, "gAlbedoSpec"), 2);
	glUniform1i(glGetUniformLocation(shaderLightningPass.program, "gAmbient"), 3);
	glUniform1i(glGetUniformLocation(shaderLightningPass.program, "depthMap"), 4);

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// - Position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - Normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - Color + Specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	//Ambient colour buffer
	glGenTextures(1, &gAmbient);
	glBindTexture(GL_TEXTURE_2D, gAmbient);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gAmbient, 0);

	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
	GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void loadModels()
{
	//Reads the models from file once
	modelLibrary.push_back(Model("models/cube/cube.obj")); //0

	//modelLibrary.push_back(Model("models/nanosuit/nanosuit.obj")); //1

	modelLibrary.push_back(Model("models/sphere/sphere.obj")); //2

	Model cannonModel = Model("models/cannon/editCannon.obj", {
		0.1, 0.0, 0.0, 0.0,
		0.0, 0.1, 0.0, 0.0,
		0.0, 0.0, 0.1, 0.0,
		2.0, -0.12, 2.0, 1.0 });
	Model cannonModel2 = Model("models/cannon/editCannon2.obj", {
		0.1, 0.0, 0.0, 0.0,
		0.0, 0.1, 0.0, 0.0,
		0.0, 0.0, 0.1, 0.0,
		2.0, -0.12, 2.0, 1.0 });
	aCannon.loadModel(cannonModel,cannonModel2);
}

void createModels()
{
	//Create the models and store them in the vector of all models to be rendered
	allModels.push_back(new Model(modelLibrary.at(0), {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 3.0, 1.0 }));

	allModels.push_back(new Model(modelLibrary.at(1), {
		0.1, 0.0, 0.0, 0.0,
		0.0, 0.1, 0.0, 0.0,
		0.0, 0.0, 0.1, 0.0,
		1.0, 0.0, 3.0, 1.0 }));
	terrain = new Terrain(60, 60, 0.1);
	terrain->loadTerrain("heightmap.bmp", 1.0f);

	//Make all models rotate at a fixed speed
	if (!allModels.empty())
	{
		glm::mat4 rotation = glm::rotate(glm::mat4(), glm::radians(0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
		for (int i = 0; i < allModels.size(); i++)
		{
			allModels[i]->setRotationMatrix(rotation);
		}
	}
	//Some lights with random values
	std::srand(13);
	for (int i = 0; i < NR_LIGHTS; i++)
	{
		//GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		//GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		//GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;

		GLfloat xPos = 2;
		GLfloat yPos = 2;
		GLfloat zPos = 4;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// Also calculate random color
		
		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.8; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.8; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.8; // Between 0.5 and 1.0
		
		//GLfloat rColor = 0.6; // Between 0.5 and 1.0
		//GLfloat gColor = 0.6; // Between 0.5 and 1.0
		//GLfloat bColor = 0.6; // Between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}
}

void setUpTweakBar()
{
	debugInterface = TwNewBar("Debug Interface");
	TwDefine(" 'Debug Interface' size='200 100' "); // resize bar
	TwDefine(" 'Debug Interface' refresh=0.1 "); // refresh the bar every 0.1 sec
	TwAddVarRW(debugInterface, "Amount of hits", TW_TYPE_INT16, &amountOfHits, "");
	TwAddVarRW(debugInterface, "Tries left", TW_TYPE_INT16, &amountOfTriesLeft, "");
}
void sort()
{
	if (!allModels.empty())
	{
		//Bubble sort
		glm::vec3 modelPos1;
		glm::vec3 modelPos2;
		bool sorted = false;
		while (!sorted)
		{
			sorted = true;
			for (int i = 0; i < allModels.size() - 1;i++)
			{
				modelPos1 = allModels[i]->getModelMatrix()[3];
				modelPos2 = allModels[i + 1]->getModelMatrix()[3];
				//Compare distance to model1 and distance to model2 and swap if out of order.
				if (glm::distance(modelPos1, playerCamera.getCameraPos()) > glm::distance(modelPos2, playerCamera.getCameraPos()))
				{
					std::swap(allModels[i], allModels[i + 1]);
					sorted = false;
				}
			}
		}
	}
}
void update(sf::Window &window)
{
	//Controls update timings
	deltaTime = deltaClock.restart();
	viewMatrix = playerCamera.Update(deltaTime.asSeconds(), window);
	playerCamera.mousePicking(window,projectionMatrix,viewMatrix,allModels);
	playerCamera.cameraFall(terrain->heightAt(playerCamera.getCameraPos().x, playerCamera.getCameraPos().z),terrain->getScale(),deltaTime.asSeconds());

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
	{
		window.setMouseCursorVisible(true);
	}
	else
	{
		window.setMouseCursorVisible(false);
	}
	aCannon.update(deltaTime.asSeconds(), lightPositions);
	for (int i = 0; i < allModels.size(); i++)
	{
		//allModels[i]->rotate();
	}
	sort();
	amountOfHits = aCannon.getAmountOfHits();
	amountOfTriesLeft = aCannon.getTriesLeft();
}

void render(sf::Window &window)
{
	//DEPTH PASS
	//Render scene from light's point of view
	glCullFace(GL_FRONT);
	depthShader.use();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	for (GLuint i = 0; i < lightPositions.size(); i++)
	{
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPositions.at(i), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		glUniformMatrix4fv(glGetUniformLocation(depthShader.program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		for (int j = 0; j < allModels.size(); j++)
		{
			glUniformMatrix4fv(glGetUniformLocation(depthShader.program, "model"), 1, GL_FALSE, &allModels[j]->getModelMatrix()[0][0]);
			allModels[j]->draw(depthShader);
		}
	}
	terrain->draw(depthShader);
	aCannon.draw(depthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);

	//Reset viewport
	glViewport(0, 0, windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//GEOMETRY PASS
	shaderGeometryPass.use();
	GLint viewID = glGetUniformLocation(shaderGeometryPass.program, "view");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
	GLint projectionID = glGetUniformLocation(shaderGeometryPass.program, "projection");
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projectionMatrix[0][0]);
	//Mouseover check
	int mouseOvered = playerCamera.mousePicking(window, projectionMatrix, viewMatrix, allModels);
	//Once to test front to back rendering
	for (int i = 0; i < allModels.size(); i++)
	{
		int isMouseOver = 0;
		if (i == mouseOvered)
		{
			isMouseOver = 1;
		}
		glUniform1i(glGetUniformLocation(shaderGeometryPass.program, "isMouseOvered"), isMouseOver);
		glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass.program, "model"), 1, GL_FALSE, &allModels[i]->getModelMatrix()[0][0]);
		allModels.at(i)->draw(shaderGeometryPass);
	}
	aCannon.draw(shaderGeometryPass);
	terrain->draw(shaderGeometryPass);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//LIGHTING PASS
	shaderLightningPass.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gAmbient);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	for (GLuint i = 0; i < lightPositions.size(); i++)
	{
		glUniform3fv(glGetUniformLocation(shaderLightningPass.program, ("lights[" + std::to_string(i) + "].Position").c_str()), 1, &lightPositions[i][0]);
		glUniform3fv(glGetUniformLocation(shaderLightningPass.program, ("lights[" + std::to_string(i) + "].Color").c_str()), 1, &lightColors[i][0]);
		// Update attenuation parameters and calculate radius
		const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const GLfloat linear = 0.7;
		const GLfloat quadratic = 1.8;
		glUniform1f(glGetUniformLocation(shaderLightningPass.program, ("lights[" + std::to_string(i) + "].Linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(shaderLightningPass.program, ("lights[" + std::to_string(i) + "].Quadratic").c_str()), quadratic);
	}
	glUniformMatrix4fv(glGetUniformLocation(shaderLightningPass.program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glUniform3fv(glGetUniformLocation(shaderLightningPass.program, "viewPos"), 1, &playerCamera.getCameraPos()[0]);


	renderQuad();
}

int main()
{
	//Create the window
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 2;
	sf::Window window(sf::VideoMode(windowWidth, windowHeight), "OpenGL", sf::Style::Default, settings);
	//Initialise AntTweakBar
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(windowWidth, windowHeight);
	setUpTweakBar();
	//V-Sync
	window.setVerticalSyncEnabled(true);
	//Disable cursor
	window.setMouseCursorVisible(false);
	//Load resources, initialize the OpenGL states, ...
	glewInit();
	//Enables depth test so vertices are drawn in the correct order
	glEnable(GL_DEPTH_TEST);
	//Create gBuffer
	createGBuffer();
	//Create models
	loadModels();
	createModels();
	//Create DepthMap
	createShadowMap();
	//Main loop
	bool running = true;
	while (running)
	{
		//Handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				//End the program
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				//Resize the viewport when the window is resized
				glViewport(0, 0, event.size.width, event.size.height);
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
					running = false;
				}
				if (event.key.code == sf::Keyboard::Return)
				{
					aCannon.shoot(playerCamera.getCameraPos(), modelLibrary[1]);
				}
			}
		}
		update(window);
		render(window);
		if(debug)TwDraw();
		//End the current frame (internally swaps the front and back buffers)
		window.display();
	}
	return 0;
}
