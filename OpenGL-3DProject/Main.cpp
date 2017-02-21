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
#include <iostream>
#include <AntTweakBar.h>
#include "Model.h"
#include "Camera.h"
#include "Shader.h"
#pragma comment(lib, "opengl32.lib")
//Initial resolutions
const int RESOLUTION_WIDTH = sf::VideoMode::getDesktopMode().width;
const int RESOLUTION_HEIGHT = sf::VideoMode::getDesktopMode().height;
const int windowWidth = 800;
const int windowHeight = 600;
bool debug = true;
float stuff = 4345435.0;
//Camera
Camera playerCamera;
//gBuffer
GLuint gBuffer;
//gBuffer Shaders
Shader shaderGeometryPass;
Shader shaderLightningPass;
//gBuffer Textures
GLuint gPosition, gNormal, gAlbedoSpec;
//Quad VAO and VBO
GLuint quadVAO = 0;
GLuint quadVBO;

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
std::vector<Model> allModels;
//AntTweakBar
TwBar *debugInterface;

void RenderQuad()
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

void CreateGBuffer()
{
	shaderGeometryPass = Shader("gBufferGeometryVertex.glsl", "gBufferGeometryFragment.glsl");
	shaderLightningPass = Shader("gBufferLightningVertex.glsl", "gBufferLightningFragment.glsl");

	// Set samplers
	shaderLightningPass.use();
	glUniform1i(glGetUniformLocation(shaderLightningPass.program, "gPosition"), 0);
	glUniform1i(glGetUniformLocation(shaderLightningPass.program, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(shaderLightningPass.program, "gAlbedoSpec"), 2);

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// - Position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 800, 600, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - Normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 800, 600, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - Color + Specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void createModels()
{
	/*
	Model matrix:
	{
		scaleX, 0.0, 0.0, 0.0,
		0.0, scaleY, 0.0, 0.0,
		0.0, 0.0, scaleZ, 0.0,
		posX, posY, posZ, 1.0
	};
	Rotation matrix is set up using glm::rotate()
	*/

	//Create the models and store them in the vector of all models
	allModels.push_back(Model("cube_green_phong_12_tris_TRIANGULATED.obj", {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0 }));
	allModels.push_back(Model("cube_green_phong_12_tris_TRIANGULATED.obj", {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		1.0, 1.0, 0.0, 1.0 }));
}

void setUpTweakBar()
{
	debugInterface = TwNewBar("Debug Interface");
	TwAddVarRW(debugInterface, "Some stuff", TW_TYPE_FLOAT, &stuff, "");
}

void update(sf::Window &window)
{
	//Controls update timings
	deltaTime = deltaClock.restart();
	viewMatrix = playerCamera.Update(deltaTime.asSeconds(), window.hasFocus());
	allModels[0].setRotationMatrix(glm::rotate(glm::mat4(), glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	allModels[0].rotate();
	allModels[1].setRotationMatrix(glm::rotate(glm::mat4(), glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	allModels[1].rotate();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
	{
		window.setMouseCursorVisible(true);
	}
	else
	{
		window.setMouseCursorVisible(false);
	}
}

void render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaderGeometryPass.use();
	GLint viewID = glGetUniformLocation(shaderGeometryPass.program, "view");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
	GLint projectionID = glGetUniformLocation(shaderGeometryPass.program, "projection");
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projectionMatrix[0][0]);
	for (int i = 0; i < allModels.size(); i++)
	{
		allModels.at(i).draw(shaderGeometryPass);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderLightningPass.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	RenderQuad();
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
	CreateGBuffer();
	//Create models
	createModels();
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
		render();
		if(debug)TwDraw();
		//End the current frame (internally swaps the front and back buffers)
		window.display();
	}
	return 0;
}