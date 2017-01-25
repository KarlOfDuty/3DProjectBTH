#include <GL\glew.h>
#include <GL\GL.h>
#include <glm\glm.hpp>
#include <SFML\Window.hpp>
#include <SFML\OpenGL.hpp>
#include <fstream>

#pragma comment(lib, "opengl32.lib")

using namespace std;

GLuint gShaderProgram = 0;

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
	shaderFile.open("Fragment.glsl");
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

int main()
{
	// create the window
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 2;

	sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	// load resources, initialize the OpenGL states, ...
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	glGenVertexArrays(1, &gVertexAttribute);
	// bind == enable
	glBindVertexArray(gVertexAttribute);
	// this activates the first and second attributes of this VAO
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	CreateShaders();

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
		}

		// clear the buffers
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw...

		// end the current frame (internally swaps the front and back buffers)
		window.display();
	}

	// release resources...

	return 0;
}