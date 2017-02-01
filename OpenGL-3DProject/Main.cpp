#include <GL\glew.h>
#include <GL\GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SFML\Window.hpp>
#include <SFML\OpenGL.hpp>
#include <fstream>
#include <iostream>

#pragma comment(lib, "opengl32.lib")

using namespace std;

int oldMouseX = 0;
int oldMouseY = 0;
float mouseX = 0;
float mouseY = 0;

float camSpeed = 0.05f;
float camPosZ = 2.0f;
float camPosX = 0.0f;

const int RESOLUTION_WIDTH = sf::VideoMode::getDesktopMode().width;
const int RESOLUTION_HEIGHT = sf::VideoMode::getDesktopMode().height;

GLuint gShaderProgram = 0;
GLuint gVertexAttribute = 0;
GLuint gVertexBuffer = 0;

GLuint gBuffer;
GLuint gPosition, gNormal, gAlbedoSpec;

int timeSinceLastFrame = 0; //DeltaTime test

//MVP PLUS ROTATION (rotation ska ändras från manuell)
glm::mat4 Model = glm::mat4(1.0f);
glm::mat4 View = glm::lookAt(
	glm::vec3(0, 0, 2),
	glm::vec3(0, 0, 0),
	glm::vec3(0, 1, 0)
);
glm::mat4 Projection = glm::perspective(45.0f, (float)800 / (float)600, 0.1f, 20.0f);

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

void CreateShaders()
{
	//create vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	// open glsl file and put it in a string
	ifstream shaderFile("VertexShader.glsl");
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

void CreateGBuffer()
{
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	int SCR_WIDTH = 800;
	int SCR_HEIGHT = 600;

	// - Position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - Normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - Color + Specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
}

void CreateTriangleData()
{
	glGenVertexArrays(1, &gVertexAttribute);
	glBindVertexArray(gVertexAttribute);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	struct TriangleVertex
	{
		float x, y, z;
		float r, g, b, a;
	};

	TriangleVertex vertices[3] = {
		{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f }
	};

	glGenBuffers(1, &gVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLint vertexPos = glGetAttribLocation(gShaderProgram, "vertexPos");
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(0));

	GLint vertexColor = glGetAttribLocation(gShaderProgram, "vertexColor");
	glVertexAttribPointer(vertexColor, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 3));
}

void Update() //Update funktion för deltaTime, Fungerar ej atm.
{
	//float deltaTime = (GL_TIME_ELAPSED - timeSinceLastFrame) / 1000;
	//timeSinceLastFrame = GL_TIME_ELAPSED;
	//Model = Model*rotation;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		camPosZ -= camSpeed;
		View = glm::lookAt(
			glm::vec3(0, 0, camPosZ),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		//Projection = Projection*glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -0.1f));
		camPosZ += camSpeed;
		View = glm::lookAt(
			glm::vec3(0, 0, camPosZ),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		camPosX -= camSpeed;
		View = glm::lookAt(
			glm::vec3(0, 0, camPosZ),
			glm::vec3(0, 0, camPosX),
			glm::vec3(0, 1, 0)
		);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		camPosX -= camSpeed;
		View = glm::lookAt(
			glm::vec3(0, 0, camPosZ),
			glm::vec3(0, 0, camPosX),
			glm::vec3(0, 1, 0)
		);
	}
	if (sf::Mouse::getPosition().x != RESOLUTION_WIDTH/2)
	{
		mouseX = (float)(sf::Mouse::getPosition().x - oldMouseX)/10.0f;
		Projection = Projection * glm::rotate(glm::mat4(), glm::radians(mouseX), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (sf::Mouse::getPosition().y != RESOLUTION_HEIGHT / 2)
	{
		mouseY = (float)(sf::Mouse::getPosition().y - oldMouseY) / 10.0f;
		//Projection = Projection * glm::rotate(glm::mat4(), glm::radians(mouseY), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	sf::Mouse::setPosition(sf::Vector2i(1920 / 2, 1080 / 2));
	oldMouseX = sf::Mouse::getPosition().x;
	oldMouseY = sf::Mouse::getPosition().y;
}

void Render()
{
	// clear the buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw...
	glUseProgram(gShaderProgram);
	glBindVertexArray(gVertexAttribute);

	GLint modelID = glGetUniformLocation(gShaderProgram, "model");
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &Model[0][0]);
	GLint viewID = glGetUniformLocation(gShaderProgram, "view");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	GLint projectionID = glGetUniformLocation(gShaderProgram, "projection");
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &Projection[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 3);
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

	//CreateGBuffer();

	CreateShaders();

	CreateTriangleData();

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

		Update();
		Render();

		// end the current frame (internally swaps the front and back buffers)
		window.display();
	}

	// release resources...

	return 0;
}