#include <GL\glew.h>
#include <GL\GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SFML\Window.hpp>
#include <SFML\OpenGL.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include "Model.h"
#include <iostream>
#include "Camera.h"
#include "Shader.h"
#pragma comment(lib, "opengl32.lib")

const int RESOLUTION_WIDTH = sf::VideoMode::getDesktopMode().width;
const int RESOLUTION_HEIGHT = sf::VideoMode::getDesktopMode().height;

Camera playerCamera;

Shader shaderProgram;

sf::Clock deltaClock;
sf::Time deltaTime;

glm::mat4 viewMatrix = glm::lookAt(
	glm::vec3(0, 0, 2),
	glm::vec3(0, 0, 0),
	glm::vec3(0, 1, 0)
);
glm::mat4 projectionMatrix = glm::perspective(45.0f, (float)800 / (float)600, 0.1f, 20.0f);
std::vector<Model> allModels;

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

void update(sf::Window &window)
{
	//Controls update timings
	deltaTime = deltaClock.restart();
	viewMatrix = playerCamera.Update(deltaTime.asSeconds(), window.hasFocus());
}

void render()
{
	//Clear the buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderProgram.use();
	GLint viewID = glGetUniformLocation(shaderProgram.program, "view");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
	GLint projectionID = glGetUniformLocation(shaderProgram.program, "projection");
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projectionMatrix[0][0]);
	//Iterate through all models and draw them
	for (int i = 0; i < allModels.size(); i++)
	{
		allModels.at(i).draw(shaderProgram);
	}
}

int main()
{
	//Create the window
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 2;
	sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, settings);
	//V-Sync
	window.setVerticalSyncEnabled(true);
	//Disable cursor
	window.setMouseCursorVisible(false);
	//Load resources, initialize the OpenGL states, ...
	glewInit();
	//Enables depth test so vertices are drawn in the correct order
	glEnable(GL_DEPTH_TEST);
	//Create shaders
	shaderProgram = Shader("VertexShader.glsl", "FragmentShader.glsl");
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
		//End the current frame (internally swaps the front and back buffers)
		window.display();
	}
	return 0;
}