#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include<fstream>
#include<sstream>
#include<iostream>
#include <GL\glew.h>
#include <GL\GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Shader.h"
//A single vertex (corner) in a face.
struct Vertex
{
	glm::vec3 pos;
	glm::vec2 texPos;
	glm::vec4 colour;
	glm::vec3 normal;
	std::string texture;
};
static bool debug = true;
class Model
{
private:
	std::vector<std::vector<Vertex>> faces;
	glm::mat4 modelMatrix;
	glm::mat4 rotationMatrix;
public:
	GLuint VAO; //Vertex Array Object
	GLuint VBO; //Vertex Buffer Object
	std::vector<std::vector<Vertex>> getFaces();
	glm::mat4 getModelMatrix();
	void rotate();
	void read(std::string filename);
	void draw(Shader shader);
	Model(std::string filename);
	Model(std::string filename, glm::mat4 modelMat);
	Model(std::string filename, glm::mat4 modelMat, glm::mat4 rotation);
	Model();
	~Model();
};
#endif