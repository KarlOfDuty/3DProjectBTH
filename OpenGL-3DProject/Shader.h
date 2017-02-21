#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
public:
	GLuint program;
	// Constructor generates the shader on the fly
	Shader();
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	// Uses the current shader
	void use();
};

#endif