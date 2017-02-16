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

//using namespace std;

const int RESOLUTION_WIDTH = sf::VideoMode::getDesktopMode().width;
const int RESOLUTION_HEIGHT = sf::VideoMode::getDesktopMode().height;

Camera playerCamera;

Shader shaderProgram;

sf::Clock deltaClock;
sf::Time deltaTime;

glm::mat4 modelMatrix = glm::mat4(1.0f);
glm::mat4 View = glm::lookAt(
	glm::vec3(0, 0, 2),
	glm::vec3(0, 0, 0),
	glm::vec3(0, 1, 0)
);
glm::mat4 Projection = glm::perspective(45.0f, (float)800 / (float)600, 0.1f, 20.0f);
std::vector<Model> allModels;

void CreateModels()
{
	//{
	//	scaleX, 0.0, 0.0, 0.0,
	//	0.0, scaleY, 0.0, 0.0,
	//	0.0, 0.0, scaleZ, 0.0,
	//	posX, posY, posZ, 1.0
	//};
	//Rotate using glm::rotate

	//Model matrices
	glm::mat4 modelMat1 = 
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	};
	glm::mat4 modelMat2 = 
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		1.0, 1.0, 1.0, 1.0
	};
	//Rotation matrices
	glm::mat4 rotation = glm::rotate(glm::mat4(), glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotation1 = glm::rotate(glm::mat4(), glm::radians(4.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//Create the models and store them in the vector of all models
	allModels.push_back(Model("cube_green_phong_12_tris_TRIANGULATED.obj", modelMat1));
	allModels.push_back(Model("cube_green_phong_12_tris_TRIANGULATED.obj", modelMat2));
}

void Update(sf::Window &window)
{
	//Controls update timings
	deltaTime = deltaClock.restart();
	View = playerCamera.Update(deltaTime.asSeconds(), window.hasFocus());
}

void Render()
{
	//Clear the buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderProgram.use();
	GLint viewID = glGetUniformLocation(shaderProgram.program, "view");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	GLint projectionID = glGetUniformLocation(shaderProgram.program, "projection");
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &Projection[0][0]);
	//Iterate through all models and draw them
	for (int i = 0; i < allModels.size(); i++)
	{
		allModels.at(i).draw(shaderProgram);
	}
}

int main()
{
	// create the window
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 2;
	sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);
	window.setMouseCursorVisible(false);
	// load resources, initialize the OpenGL states, ...
	glewInit();
	glEnable(GL_DEPTH_TEST);
	shaderProgram = Shader("VertexShader.glsl", "FragmentShader.glsl");

	CreateModels();

	// run the main loop
	bool running = true;
	while (running)
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				// end the program
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				// adjust the viewport when the window is resized
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

		Update(window);
		Render();

		// end the current frame (internally swaps the front and back buffers)
		window.display();
	}

	// release resources...

	return 0;
}