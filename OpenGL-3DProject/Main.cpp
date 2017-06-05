
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
#include <SOIL.h>
#include "Model.h"
#include "Camera.h"
#include "Shader.h"
#include "FrustumCulling.h"
#pragma comment(lib, "opengl32.lib")
///////////////////////////////////////
//Toggle for demo, demo camera only moves in 2D, while normal occlusion works in 3D
const bool aboveView = false;
const bool frustumCulling = true;
///////////////////////////////////////

//Initial resolutions
const int RESOLUTION_WIDTH = sf::VideoMode::getDesktopMode().width;
const int RESOLUTION_HEIGHT = sf::VideoMode::getDesktopMode().height;
const int windowWidth = 1280;
const int windowHeight = 720;
bool debug = false;
//Camera
Camera playerCamera = Camera();
//Frustum culling object
FrustumCulling frustumObject = FrustumCulling();
float fov = 45.0f;
float nearPlane = 0.1f;
float farPlane = 1000.0f;
glm::mat4 projectionMatrix = glm::perspective(fov, (float)windowWidth / (float)windowHeight, nearPlane, farPlane);
//gBuffer
GLuint gBuffer;
//gBuffer Shaders
Shader shaderGeometryPass;
Shader shaderLightningPass;
//gBuffer Textures
GLuint gPosition, gNormal, gAlbedoSpec, gAmbient;
//Quad VAO and VBO
GLuint quadVAO = 0;
GLuint quadVBO;
//Temp values for textures and all the lights
GLuint diffuseTexture;
GLuint specularTexture;
GLuint normalMap;
const GLuint NR_LIGHTS = 1;
std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;
//Timing control for controls and camera
sf::Clock deltaClock;
sf::Time deltaTime;
//Matrices
glm::mat4 viewMatrix = glm::lookAt(
	glm::vec3(0, 100, 0),
	glm::vec3(0, 0, 0),
	glm::vec3(0, 1, 0));
//All models in the program
std::vector<Model*> allModels;
std::vector<Model> modelLibrary;
std::vector<Model*> visibleModels;
void renderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = 
		{
			//Positions        //Texture Coords
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

	modelLibrary.push_back(Model("models/sphere/sphere.obj")); //1
}

void createModels()
{
	std::srand(time(0));
	glm::mat4 rot = glm::rotate(glm::mat4(), 0.1f, glm::vec3(0, 1, 0));
	//Loads 100 spheres randomly
	for (int i = 0; i < 10000; i++)
	{
		allModels.push_back(new Model(modelLibrary[0], {
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			(rand() % 100)-50, (rand() % 10)-5, (rand() % 100)-50, 1.0 },rot));
	}
	//Some lights with random values
	std::srand(13);
	for (int i = 0; i < NR_LIGHTS; i++)
	{
		GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// Also calculate random color
		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}
	visibleModels = allModels;
}

void update(sf::Window &window)
{
	//Controls update timings
	deltaTime = deltaClock.restart();
	if (aboveView)
	{
		playerCamera.Update(deltaTime.asSeconds(), window);
		viewMatrix = glm::lookAt(
			glm::vec3(0, 100, 0),
			glm::vec3(1, 1, 1),
			glm::vec3(1, 0, 0));
	}
	else
	{
		viewMatrix = playerCamera.Update(deltaTime.asSeconds(), window);
	}
	if(frustumCulling)playerCamera.frustumCulling(frustumObject, visibleModels);
	//playerCamera.mousePicking(window,projectionMatrix,viewMatrix,allModels);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
	{
		window.setMouseCursorVisible(true);
	}
	else
	{
		window.setMouseCursorVisible(false);
	}
	//for (int i = 0; i < allModels.size(); i++)
	//{
	//	allModels[i]->rotate();
	//}
}
void render(sf::Window &window)
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Geometry pass
	shaderGeometryPass.use();
	GLint viewID = glGetUniformLocation(shaderGeometryPass.program, "view");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
	GLint projectionID = glGetUniformLocation(shaderGeometryPass.program, "projection");
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projectionMatrix[0][0]);
	//Mouseover check
	int mouseOvered = playerCamera.mousePicking(window, projectionMatrix, viewMatrix, allModels);
	//Once to test front to back rendering
	for (int i = 0; i < visibleModels.size(); i++)
	{
		int isMouseOver = 0;
		if (i == mouseOvered)
		{
			isMouseOver = 1;
		}
		glUniform1i(glGetUniformLocation(shaderGeometryPass.program, "isMouseOvered"), isMouseOver);
		glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass.program, "model"), 1, GL_FALSE, &visibleModels[i]->getModelMatrix()[0][0]);
		visibleModels.at(i)->draw(shaderGeometryPass);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Lighting pass
	shaderLightningPass.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gAmbient);

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
	//V-Sync
	window.setVerticalSyncEnabled(false);
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
	//Set up the frustum culling object and quadtree
	frustumObject.setFrustumShape(fov, (float)windowWidth / (float)windowHeight, nearPlane, farPlane);
	glm::vec4 mapSize = glm::vec4(-100.0f, -100.0f, 100.0f, 100.0f);
	frustumObject.getRoot()->buildQuadTree(allModels, 0, mapSize);
	frustumObject.getRoot()->cleanTree();
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
			}
		}
		update(window);
		render(window);
		//End the current frame (internally swaps the front and back buffers)
		window.display();
	}
	return 0;
}
