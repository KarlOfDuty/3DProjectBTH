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
#pragma comment(lib, "opengl32.lib")

//using namespace std;

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

const int RESOLUTION_WIDTH = sf::VideoMode::getDesktopMode().width;
const int RESOLUTION_HEIGHT = sf::VideoMode::getDesktopMode().height;

Camera playerCamera;

GLuint gShaderProgram = 0;
GLuint gVertexAttribute = 0;
GLuint gVertexBuffer = 0;

GLuint VAO2;

sf::Clock deltaClock;
sf::Time deltaTime;

glm::mat4 modelMatrix = glm::mat4(1.0f);
glm::mat4 View = glm::lookAt(
	glm::vec3(0, 0, 2),
	glm::vec3(0, 0, 0),
	glm::vec3(0, 1, 0)
);
glm::mat4 Projection = glm::perspective(45.0f, (float)800 / (float)600, 0.1f, 20.0f);
glm::mat4 rotation = glm::rotate(glm::mat4(), glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
int numVertices = 0;
std::vector<Model> allModels;


void CreateShaders()
{
	//create vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	// open glsl file and put it in a string
	std::ifstream shaderFile("VertexShader.glsl");
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	// make a double pointer (only valid here)
	const char* shaderTextPtr = shaderText.c_str();
	// ask GL to load this
	glShaderSource(vs, 1, &shaderTextPtr, nullptr);
	// ask GL to compile it
	glCompileShader(vs);

	//create fragment shader | same process.
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFile.open("FragmentShader.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	glCompileShader(fs);

	GLint isCompiled = 0;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		// The maxLength includes the NULL character
		GLchar log_string[1024] = {};
		glGetShaderInfoLog(vs, 1024, nullptr, log_string);
		OutputDebugStringA((char*)log_string);

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(vs); // Don't leak the shader.
		return;
	}

	//link shader program (connect vs and ps)
	gShaderProgram = glCreateProgram();
	glAttachShader(gShaderProgram, fs);
	glAttachShader(gShaderProgram, vs);
	glLinkProgram(gShaderProgram);

	GLint isLinked = 0;
	glGetProgramiv(gShaderProgram, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(gShaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		GLchar log_string[1024] = {};
		glGetProgramInfoLog(gShaderProgram, 1024, nullptr, log_string);
		OutputDebugStringA((char*)log_string);

		//We don't need the program anymore.
		glDeleteProgram(gShaderProgram);
		//Use the infoLog as you see fit.

		//In this simple program, we'll just leave
		return;
	}
}

void rotateModels()
{
	allModels.at(0).rotate(rotation);
}
void CreateModels()
{
	std::vector<Vertex> tempTest = std::vector<Vertex>();
	//Iterate through all models
	for (int i = 0; i < allModels.size(); i++)
	{
		//Iterate through all faces
		for (int j = 0; j < allModels.at(i).getFaces().size(); j++)
		{
			//Iterate through vertices in the face
			for (int k = 0; k < 3; k++)
			{
				tempTest.push_back(allModels.at(i).getFaces().at(j).at(k));
				numVertices++;
			}
		}
	}
	//
	glGenVertexArrays(1, &gVertexAttribute);
	glBindVertexArray(gVertexAttribute);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &gVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, tempTest.size()* sizeof(Vertex), &tempTest.front(), GL_STATIC_DRAW);
	GLint vertexPos = glGetAttribLocation(gShaderProgram, "vertexPos");
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
	GLint vertexColor = glGetAttribLocation(gShaderProgram, "vertexColor");
	glVertexAttribPointer(vertexColor, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 5));
	GLint vertexModelMatrix = glGetAttribLocation(gShaderProgram, "modelMatrix");
	glVertexAttribPointer(vertexModelMatrix, 16, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 12));
}

void Update(sf::Window &window)
{
	deltaTime = deltaClock.restart();
	View = playerCamera.Update(deltaTime.asSeconds(), window.hasFocus());
	rotateModels();
}

void Render()
{
	// clear the buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw...
	glUseProgram(gShaderProgram);
	glBindVertexArray(gVertexAttribute);

	GLint modelID = glGetUniformLocation(gShaderProgram, "model");
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &allModels.at(0).getModelMatrix()[0][0]);
	GLint viewID = glGetUniformLocation(gShaderProgram, "view");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	GLint projectionID = glGetUniformLocation(gShaderProgram, "projection");
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &Projection[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
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
	allModels.push_back(Model("cube_green_phong_12_tris_TRIANGULATED.obj"));
	// load resources, initialize the OpenGL states, ...
	glewInit();

	CreateShaders();

	rotateModels();

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